; ModuleID = 'loop_nest_for.bc'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-redhat-linux-gnu"

@x = global i32 0

define i32 @loopnest() {
loopnest:
  %sum = alloca i32
  %i = alloca i32
  %j = alloca i32
  store i32 0, i32* %sum
  store i32 0, i32* %i
  %i1 = load i32* %i
  %i2 = load i32* %i
  %0 = icmp slt i32 %i2, 10
  br i1 %0, label %body, label %footer

body3:                                            ; No predecessors!
  store i32 0, i32* %j
  %j5 = load i32* %j
  %j6 = load i32* %j
  %1 = icmp slt i32 %j6, 2
  br i1 %1, label %body8, label %footer7

footer:                                           ; preds = %loopnest
  %x = load i32* @x
  %x11 = load i32* @x
  %sum12 = load i32* %sum
  %sum13 = load i32* %sum
  %x14 = load i32* @x
  %sum15 = load i32* %sum
  %2 = add i32 %sum15, %x14
  store i32 %2, i32* %sum
  br label %step

body:                                             ; preds = %loopnest
  %j18 = load i32* %j
  %j19 = load i32* %j
  %j20 = load i32* %j
  %"int++" = add i32 %j20, 1
  store i32 %"int++", i32* %j
  %j21 = load i32* %j
  %j22 = load i32* %j
  %3 = icmp slt i32 %j22, 2
  br i1 %3, label %body24, label %footer23

body4:                                            ; No predecessors!
  br label %step27

body9:                                            ; No predecessors!
  %i30 = load i32* %i
  %i31 = load i32* %i
  %i32 = load i32* %i
  %"int++33" = add i32 %i32, 1
  store i32 %"int++33", i32* %i
  %i34 = load i32* %i
  %i35 = load i32* %i
  %4 = icmp slt i32 %i35, 10
  br i1 %4, label %body37, label %footer36

footer7:                                          ; preds = %body3
  %sum40 = load i32* %sum
  ret i32 %sum40

body8:                                            ; preds = %body3

body10:                                           ; No predecessors!

step16:                                           ; No predecessors!

step:                                             ; preds = %footer

step17:                                           ; No predecessors!

footer25:                                         ; No predecessors!

footer23:                                         ; preds = %body

body24:                                           ; preds = %body

footer26:                                         ; No predecessors!

step28:                                           ; No predecessors!

step27:                                           ; preds = %body4

step29:                                           ; No predecessors!

footer38:                                         ; No predecessors!

footer36:                                         ; preds = %body9

body37:                                           ; preds = %body9

footer39:                                         ; No predecessors!
}
