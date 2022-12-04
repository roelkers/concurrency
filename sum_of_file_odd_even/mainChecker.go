package main

import (
  "fmt"
  "sync"
)

func printer(c chan int, result chan string, group *sync.WaitGroup) {
   // for {
     var integer int
     integer = <- c
     fmt.Println(integer) 
     result <- "THE END"
     group.Done()
   // }
}

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
