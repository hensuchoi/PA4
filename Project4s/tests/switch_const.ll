; ModuleID = 'switch_const.bc'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-redhat-linux-gnu"

define i32 @main() {
main:
  %x = alloca i32
  store i32 11, i32* %x
  %x1 = load i32* %x
  switch i32 %x1, label %default [
  ]

body:                                             ; No predecessors!
  br label %default

footer:                                           ; No predecessors!
  br label %case

default:                                          ; preds = %body, %main
  br label %case

case:                                             ; preds = %default, %footer
}
