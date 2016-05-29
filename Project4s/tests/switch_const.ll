; ModuleID = 'switch_const.bc'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-redhat-linux-gnu"

define i32 @main() {
main:
  %x = alloca i32
  store i32 11, i32* %x
  br label %case1

case:                                             ; No predecessors!
  br label %case3

case1:                                            ; preds = %main
  br label %default4

case2:                                            ; No predecessors!
  br label %footer

case3:                                            ; preds = %case

default:                                          ; No predecessors!

default4:                                         ; preds = %case1

default5:                                         ; No predecessors!

default6:                                         ; No predecessors!

footer:                                           ; preds = %case2

footer7:                                          ; No predecessors!
}
