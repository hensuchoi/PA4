; ModuleID = 'loop_nest_while.bc'
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
  br label %test

test1:                                            ; No predecessors!
  %i5 = load i32* %i
  %i6 = load i32* %i
  %0 = icmp slt i32 %i6, 10
  br i1 %0, label %body, label %footer

test:                                             ; preds = %loopnest
  store i32 0, i32* %j
  br label %test10

test2:                                            ; No predecessors!
  %j15 = load i32* %j
  %j16 = load i32* %j
  %1 = icmp slt i32 %j16, 2
  br i1 %1, label %body18, label %footer17

test3:                                            ; No predecessors!
  %x = load i32* @x
  %sum22 = load i32* %sum
  %x23 = load i32* @x
  %sum24 = load i32* %sum
  %"int+" = add i32 %sum24, %x23
  %x25 = load i32* @x
  %sum26 = load i32* %sum
  %x27 = load i32* @x
  %sum28 = load i32* %sum
  %"int+29" = add i32 %sum28, %x27
  %x30 = load i32* @x
  %sum31 = load i32* %sum
  %x32 = load i32* @x
  %sum33 = load i32* %sum
  %"int+34" = add i32 %sum33, %x32
  store i32 %"int+34", i32* %sum
  %x35 = load i32* @x
  %sum36 = load i32* %sum
  %x37 = load i32* @x
  %sum38 = load i32* %sum
  %"int+39" = add i32 %sum38, %x37
  %j40 = load i32* %j
  %j41 = load i32* %j
  %"int+42" = add i32 %j41, 1
  %j43 = load i32* %j
  %j44 = load i32* %j
  %"int+45" = add i32 %j44, 1
  %j46 = load i32* %j
  %j47 = load i32* %j
  %"int+48" = add i32 %j47, 1
  store i32 %"int+48", i32* %j
  %j49 = load i32* %j
  %j50 = load i32* %j
  %"int+51" = add i32 %j50, 1
  br label %test56

test4:                                            ; No predecessors!
  br label %test54

footer:                                           ; preds = %test1
  %i58 = load i32* %i
  %i59 = load i32* %i
  %"int+60" = add i32 %i59, 1
  %i61 = load i32* %i
  %i62 = load i32* %i
  %"int+63" = add i32 %i62, 1
  %i64 = load i32* %i
  %i65 = load i32* %i
  %"int+66" = add i32 %i65, 1
  store i32 %"int+66", i32* %i
  %i67 = load i32* %i
  %i68 = load i32* %i
  %"int+69" = add i32 %i68, 1
  br label %test74

body:                                             ; preds = %test1
  br label %test72

test7:                                            ; No predecessors!
  %sum76 = load i32* %sum
  ret i32 %sum76

body8:                                            ; No predecessors!

body9:                                            ; No predecessors!

test11:                                           ; No predecessors!

test10:                                           ; preds = %test

test12:                                           ; No predecessors!

test13:                                           ; No predecessors!

test14:                                           ; No predecessors!

footer17:                                         ; preds = %test2

body18:                                           ; preds = %test2

test19:                                           ; No predecessors!

body20:                                           ; No predecessors!

body21:                                           ; No predecessors!

footer55:                                         ; No predecessors!

body52:                                           ; No predecessors!

body53:                                           ; No predecessors!

test54:                                           ; preds = %test4

test56:                                           ; preds = %test3

footer57:                                         ; No predecessors!

footer73:                                         ; No predecessors!

body70:                                           ; No predecessors!

body71:                                           ; No predecessors!

test72:                                           ; preds = %body

test74:                                           ; preds = %footer

footer75:                                         ; No predecessors!
}
