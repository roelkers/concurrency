package main

import (
	"math/rand"
	"fmt"
	"os"
)


func main() {
  max := 1000

  file, err := os.OpenFile("testFile1", os.O_CREATE, 0600) 
  if err != nil {
    fmt.Println("could not open file")
    return
  }
  var number int
  var seq []byte
  for i := 0; i < max; i++ {
    
    number = rand.Intn(10)
    seq = []byte(fmt.Sprintf("%i\n", number))
    fmt.Println(seq)
    _, err := file.Write([]byte(fmt.Sprintf("%i\n", number))) 
    if err != nil {
      fmt.Println(err)
      return
    }
  }
  //file.Write([]byte("ASDASD")) 
  file.Close()
  
}
