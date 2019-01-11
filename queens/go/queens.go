package main

import (
	"flag"
	"fmt"
	"log"
	"os"
	"runtime/pprof"
)

type bitsType uint32

// flags
var boardSize = flag.Int("size", 8, "the board size")
var cpuprofile = flag.String("cpuprofile", "", "write cpu profile to file")

// globals
var done bitsType

func main() {
	flag.Parse()

	fmt.Printf("Find all solutions to the %d-Queens Problems (v2)\n", *boardSize)

	done = bitsType((1 << uint32(*boardSize)) - 1)

	if *cpuprofile != "" {
		f, err := os.Create(*cpuprofile)
		if err != nil {
			log.Fatal(err)
		}
		pprof.StartCPUProfile(f)
		defer pprof.StopCPUProfile()
	}

	solutions := countSolutions(bitsType(0), bitsType(0), bitsType(0))
	fmt.Printf("Found %d solutions\n", solutions)

}

func countSolutions(col, ld, rd bitsType) int {
	solutions := 0
	if col == done {
		return 1
	}

	excl := (col | ld | rd) & done
	if excl == done {
		return 0
	}

	avail := ^excl & done
	for avail > 0 {
		newQueen := avail & -avail
		solutions += countSolutions(col|newQueen, (ld|newQueen)<<1, (rd|newQueen)>>1)
		avail -= newQueen
	}
	return solutions
}
