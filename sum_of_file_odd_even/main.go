package main

import (
  "fmt"
  "sync"
)

func mainChecker() {
  c := make(chan int, 10)
  result := make(chan string, 10)
  var group sync.WaitGroup
  group.Add(1)

  go printer(c, result, &group)
  c <- 5 
  group.Wait()
  // c <- 10
  // final = <- result
  // c <- 2
  // final = <- result

  //fmt.Print(final)
  
}
