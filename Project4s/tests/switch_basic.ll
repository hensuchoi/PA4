; ModuleID = 'switch_basic.bc'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-redhat-linux-gnu"

@a = global i32 0

define float @switchtest() {
switchtest:
  %f = alloca float
  br label %case1

case:                                             ; No predecessors!
  br label %case3

case1:                                            ; preds = %switchtest
  br label %case5

case2:                                            ; No predecessors!
  br label %default6

case3:                                            ; preds = %case
  br label %footer

case4:                                            ; No predecessors!
  %f10 = load float* %f
  ret float %f10

case5:                                            ; preds = %case1

default:                                          ; No predecessors!

default6:                                         ; preds = %case2

default7:                                         ; No predecessors!

default8:                                         ; No predecessors!

footer:                                           ; preds = %case3

footer9:                                          ; No predecessors!
}
