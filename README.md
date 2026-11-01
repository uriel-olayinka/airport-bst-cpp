# Airport Code Lookup Service (Binary Search Tree)

EECE 7205 Fall 2026 mini project. Author: Uriel Olayinka

## The problem
Airports are identified by a 3-letter IATA code (BOS, JFK, DAC). This program
loads about 6,000 real airports into a binary search tree keyed by IATA code,
so it can look up an airport by code, list airports alphabetically, answer range
queries (every code from BOS to BWI), and stay correct as airports are added and
removed. The tree is my own implementation, in C++.

## Data
Airport data from OpenFlights.org (openflights.org/data.php), published under the Open Database License. The file in `data/` is the copy posted on the course page.

## Build and run
```
g++ -std=c++17 -O2 src/*.cpp -o airports
./airports
```

## Commands
| Command | What it does |
| --- | --- |
| `load FILE` | read the airport file and build the tree (file order) |
| `find CODE` | print the airport, the search path and the comparison count |
| `minmax` | print the smallest and largest code |
| `add CODE name\|city\|country` | insert an airport |
| `remove CODE` | delete an airport (cases D1, D2, D3) and validate |
| `list [N]` | print airports in alphabetical order (all, or the first N) |
| `save FILE` / `loadtree FILE` | write the codes in preorder / rebuild the same tree |
| `range FROM TO` | list airports with FROM <= code <= TO and count nodes visited |
| `validate` | check the BST property with the (lo, hi) range method |
| `levels` | print the top three levels of the tree |
| `rebalance` | rebuild as a balanced tree (middle element as root) |
| `experiment` | the shape experiment below |
| `check` | confirms the inorder ist is sorted |
| `quit` or `exit` | exits the program |

## Sample session
```
> load data/airports.csv
Loaded 6072 airports (1626 lines skipped: no IATA code). Duplicates: 0
Height: 177   Average depth: 18.3
> find BOS
Path: GKA -> AEY -> EGS -> BUJ -> BJA -> BSK -> BOY -> BNI -> BOH -> BOO -> BOU -> BOS    (comparisons: 12)
BOS General Edward Lawrence Logan International Airport, Boston, United States
> range BOS BWI
113 airports, 127 nodes visited
(list of all airports has been omitted)
> remove MAG
Before:
  L0: GKA
  L1: AEY MAG
  L2: AAE EGS HGU POM
After:
  L0: GKA
  L1: AEY MAH
  L2: AAE EGS HGU POM
Deleted MAG (case D3: two children, replaced by successor MAH)   validate: OK
> validate
validate: OK
```

## Shape experiment (Task 6)
Same 6,072 codes, built in different orders. Times are 10,000 random searches
(best of 5 runs) on my MacBook.

| Build order | Height | Average depth | Time for 10,000 searches | Explanation |
| --- | --- | --- | --- | --- |
| File order | 177 | 18.3 | 3.439 | The file is grouped by country, and inside a country the codes are often already in alphabetical order. Each alphabetical stretch behaves like a small sorted insert and grows a long one-sided branch. The longest path has 177 levels, and the average search takes about 19.3 steps. |
| Sorted order | 6071 | 3035.5 | 214.488 | Every new code is bigger than all the codes already inserted, so it always goes to the right. The tree becomes a single chain of 6,072 nodes (height n − 1). An average search walks about 3,036 nodes, which is why it is about 60 times slower than file order and about 190 times slower than balanced. |
| Balanced (middle rule) | 12 | 10.7 | 1.132 | Using the middle code as the root, and repeating on each half, keeps both sides equal at every step. The height is ⌊log₂ 6072⌋ = 12, the smallest possible for this many nodes. An average search takes about 11.7 steps, the fewest of any row. |
| Random shuffle (bonus) | 28 | 13.6 | 1.534 | In random order, long one-sided stretches are highly unlikely, so the tree comes out reasonably balanced. The height is about twice the balanced height, and the average search takes about 14.6 steps. |

The file order is much worse than a random shuffle because a random shuffle spreads the codes out evenly, so no branch grows much longer than the rest, and the height stays near 28. The file is not random. It lists airports country by country, and within a country the codes are often already alphabetical. Inserting an alphabetical run is like inserting sorted data, so each run grows a long one-sided chain. This gives a height of 177, about six times the random height. The average depth is only moderately worse (18.3 versus 13.6), because most nodes sit in the shallower parts of the tree, and the long chains have a minority of the nodes.

## Design notes
- `Airport` is the record, `AirportBST` is the tree, with its root private.
- Insert, search, delete, validate and the traversals use loops and an explicit
  stack or queue, not recursion, because sorted input makes a tree 6,071 levels deep.
  The balanced build uses recursion; it is safe because its depth is ony about 13 levels.
- Delete case D3 copies in the inorder successor and then removes that node.
- Pseudocode for every operation is in `docs/pseudocode.md`.