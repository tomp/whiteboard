package main

import (
	"flag"
	"fmt"
	"log"
	"os"
	"runtime/pprof"
)

type bitsType uint32

type stateType struct {
	sz  uint32   // board size
	col bitsType // position is occupied by a queen
	rd  bitsType // position is attacked from the left
	ld  bitsType // position is attacked from the right
}

// flags
var boardSize = flag.Int("size", 8, "the board size")
var cpuprofile = flag.String("cpuprofile", "", "write cpu profile to file")

// globals
var done bitsType

func main() {
	flag.Parse()

	fmt.Printf("Find all solutions to the %d-Queens Problems\n", *boardSize)

	done = bitsType((1 << uint32(*boardSize)) - 1)

	if *cpuprofile != "" {
		f, err := os.Create(*cpuprofile)
		if err != nil {
			log.Fatal(err)
		}
		pprof.StartCPUProfile(f)
		defer pprof.StopCPUProfile()
	}

	initState := newState(uint32(*boardSize))
	solutions := countSolutions(initState)
	fmt.Printf("Found %d solutions\n", solutions)

}

// newState returns a newly allocated, zero-valued stateType.
func newState(size uint32) stateType {
	return stateType{size, 0, 0, 0}
}

// addQueen returns a new stateType, created from the given stateType by
// adding a new queen at the position indicated by the newQueen bit mask.
// It's assumed that only one bit of newQueen is non-zero, and that (s.col & newQueen) is zero.
func addQueen(s *stateType, newQueen bitsType) stateType {
	return stateType{s.sz,
		s.col | newQueen,
		(s.rd | newQueen) >> 1,
		(s.ld | newQueen) << 1}
}

func countSolutions(s stateType) int {
	solutions := 0
	excl := s.col | s.ld | s.rd
	newQueen := bitsType(1 << (s.sz - 1))
	for newQueen != 0 {
		if (newQueen & excl) == 0 {
			newState := addQueen(&s, newQueen)
			if newState.col == done {
				return 1
			}
			solutions += countSolutions(newState)
		}
		newQueen = newQueen >> 1
	}
	return solutions
}
