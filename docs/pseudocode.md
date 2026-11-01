LOAD(path)
  stream <- open(path)
  skipped <- 0
  list <- empty vector<Airport>

  for each line in stream do
    if line is empty or is_header(line) then
      continue
    
    // Parse CSV respecting double-quoted strings containing commas
    fields <- SPLIT_CSV(line)
    iata <- fields[4]

    // Skip missing IATA codes ("\N") or malformed entries
    if iata == "\N" or length(iata) != 3 then
      skipped <- skipped + 1
      continue

    record <- Airport(fields[1..7])
    append record to list

  return (list, skipped)


BUILD(list)
  tree.root <- NULL
  tree.size <- 0
  duplicates <- 0
  n <- length(list)

  for i <- 0 to n - 1 do
    if not INSERT(tree, list[i]) then
      duplicates <- duplicates + 1
      print "Duplicate IATA key rejected: ", list[i].code

  (h, avg_depth) <- HEIGHT and AVERAGE_DEPTH(tree)
  print tree.size, duplicates, h, avg_depth


INSERT(tree, a)
  if tree.root == NULL then
    tree.root <- new Node(a)
    tree.size <- 1
    return true

  cur <- tree.root
  while true do
    if a.code == cur.code then
      return false  // Rule I4: reject duplicate key
    else if a.code < cur.code then
      if cur.left == NULL then
        cur.left <- new Node(a)
        tree.size <- tree.size + 1
        return true
      cur <- cur.left
    else
      if cur.right == NULL then
        cur.right <- new Node(a)
        tree.size <- tree.size + 1
        return true
      cur <- cur.right


HEIGHT and AVERAGE_DEPTH
  if tree.root == NULL then
    return (-1, 0.0)

  queue <- empty queue of (Node*, int)
  enqueue(queue, (tree.root, 0))
  max_depth <- 0
  total_depth <- 0

  while not is_empty(queue) do
    (u, d) <- dequeue(queue)
    max_depth <- max(max_depth, d)
    total_depth <- total_depth + d

    if u.left != NULL then enqueue(queue, (u.left, d + 1))
    if u.right != NULL then enqueue(queue, (u.right, d + 1))

  height <- max_depth
  avg_depth <- total_depth / tree.size
  return (height, avg_depth)


SEARCH(tree, code)
  cur <- tree.root
  comparisons <- 0
  path <- empty list of string

  while cur != NULL do
    comparisons <- comparisons + 1
    append cur.code to path

    if code == cur.code then
      return (cur, path, comparisons)
    else if code < cur.code then
      cur <- cur.left
    else
      cur <- cur.right

  append "NULL" to path
  return (NULL, path, comparisons)


MIN / MAX
  MIN:
    cur <- tree.root
    while cur != NULL and cur.left != NULL do
      cur <- cur.left
    return cur

  MAX:
    cur <- tree.root
    while cur != NULL and cur.right != NULL do
      cur <- cur.right
    return cur


DELETE(tree, code)
  parent <- NULL
  cur <- tree.root

  // Locate target node and its parent
  while cur != NULL and cur.code != code do
    parent <- cur
    if code < cur.code then cur <- cur.left else cur <- cur.right

  if cur == NULL then
    print "Key not found: ", code
    return false

  // Case D3: Target has two children
  if cur.left != NULL and cur.right != NULL then
    succ_parent <- cur
    succ <- cur.right
    while succ.left != NULL do
      succ_parent <- succ
      succ <- succ.left
    
    cur.data <- succ.data
    cur <- succ
    parent <- succ_parent

  // Cases D1 (leaf) and D2 (single child)
  child <- (cur.left != NULL) ? cur.left : cur.right

  if parent == NULL then
    tree.root <- child
  else if parent.left == cur then
    parent.left <- child
  else
    parent.right <- child

  free(cur)
  tree.size <- tree.size - 1
  return true


VALIDATE(tree)   // range method
  if tree.root == NULL then return true

  stack <- empty stack of (Node*, string*, string*)
  push(stack, (tree.root, NULL, NULL))

  while not is_empty(stack) do
    (u, lo, hi) <- pop(stack)

    if lo != NULL and u.code <= *lo then return false
    if hi != NULL and u.code >= *hi then return false

    if u.left != NULL then push(stack, (u.left, lo, &u.code))
    if u.right != NULL then push(stack, (u.right, &u.code, hi))

  return true


INORDER (sorted)
  stack <- empty stack of Node*
  cur <- tree.root

  while cur != NULL or not is_empty(stack) do
    while cur != NULL do
      push(stack, cur)
      cur <- cur.left
    cur <- pop(stack)
    visit(cur)
    cur <- cur.right

PREORDER:
  if tree.root == NULL then return
  stack <- [tree.root]
  while not is_empty(stack) do
    u <- pop(stack)
    visit(u)
    if u.right != NULL then push(stack, u.right)
    if u.left != NULL then push(stack, u.left)

LEVEL ORDER:
  if tree.root == NULL then return
  queue <- [tree.root]
  while not is_empty(queue) do
    u <- dequeue(queue)
    visit(u)
    if u.left != NULL then enqueue(queue, u.left)
    if u.right != NULL then enqueue(queue, u.right)


SAVE(file):
  stream <- open_write(file)
  for each node u in PREORDER(tree) do
    write_line(stream, u.code)

LOADTREE(file):
  stream <- open_read(file)
  tree.root <- NULL
  for each code in stream do
    record <- lookup(code, master_list)
    INSERT(tree, record)


RANGE(from, to)
  visited <- 0
  matches <- 0

  // Pruned recursive/stack-based helper
  PRUNED_INORDER(u):
    if u == NULL then return
    visited <- visited + 1

    if u.code > from then
      PRUNED_INORDER(u.left)
    if from <= u.code and u.code <= to then
      print u.data
      matches <- matches + 1
    if u.code < to then
      PRUNED_INORDER(u.right)

  PRUNED_INORDER(tree.root)
  print matches, visited


BALANCED_BUILD(sorted list a[0..n-1])
  BUILD_SUBTREE(lo, hi):
    if lo > hi then return NULL
    mid <- lo + (hi - lo) / 2
    u <- new Node(a[mid])
    u.left <- BUILD_SUBTREE(lo, mid - 1)
    u.right <- BUILD_SUBTREE(mid + 1, hi)
    return u

  REBALANCE:
    sorted_records <- collect_records_via_INORDER(tree)
    clear_nodes(tree.root)
    tree.root <- BUILD_SUBTREE(0, length(sorted_records) - 1)