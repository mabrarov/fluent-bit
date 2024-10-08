package main

import (
	"fmt"
	"time"
	"unsafe"

	"github.com/valyala/fastjson"
)

//export go_filter
func go_filter(tag *uint8, tag_len uint, time_sec uint, time_nsec uint, record *uint8, record_len uint) *uint8 {
	btag := unsafe.Slice(tag, tag_len)  // Note, requires Go 1.17 (tinygo 0.20)
	brecord := unsafe.Slice(record, record_len)
	now := time.Unix(int64(time_sec), int64(time_nsec))

	br := string(brecord)
	var p fastjson.Parser
	value, err := p.Parse(br)
	if err != nil {
		fmt.Println(err)
		return nil
	}
	obj, err := value.Object()
	if err != nil {
		fmt.Println(err)
		return nil
	}

	var arena fastjson.Arena
	obj.Set("time", arena.NewString(now.String()))
	obj.Set("tag", arena.NewString(string(btag)))
	obj.Set("original", arena.NewString(br))
	
	s := obj.String() + string(rune(0)) 
	rv := []byte(s)

	return &rv[0]
}

func main() {}