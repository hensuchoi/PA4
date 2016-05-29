; ModuleID = 'while_loop.bc'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-redhat-linux-gnu"

@b = global i32 0

define i32 @whiletest() {
whiletest:
  %i = alloca i32
  %sum = alloca i32
  store i32 0, i32* %i
  store i32 0, i32* %sum
  br label %test

test1:                                            ; No predecessors!
  %i5 = load i32* %i
  %b = load i32* @b
  %b6 = load i32* @b
  %i7 = load i32* %i
  %0 = icmp slt i32 %i7, %b6
  br i1 %0, label %body, label %footer

test:                                             ; preds = %whiletest
  %i11 = load i32* %i
  %sum12 = load i32* %sum
  %i13 = load i32* %i
  %sum14 = load i32* %sum
  %"int+" = add i32 %sum14, %i13
  %i15 = load i32* %i
  %sum16 = load i32* %sum
  %i17 = load i32* %i
  %sum18 = load i32* %sum
  %"int+19" = add i32 %sum18, %i17
  %i20 = load i32* %i
  %sum21 = load i32* %sum
  %i22 = load i32* %i
  %sum23 = load i32* %sum
  %"int+24" = add i32 %sum23, %i22
  store i32 %"int+24", i32* %sum
  %i25 = load i32* %i
  %sum26 = load i32* %sum
  %i27 = load i32* %i
  %sum28 = load i32* %sum
  %"int+29" = add i32 %sum28, %i27
  %i30 = load i32* %i
  %i31 = load i32* %i
  %"int+32" = add i32 %i31, 1
  %i33 = load i32* %i
  %i34 = load i32* %i
  %"int+35" = add i32 %i34, 1
  %i36 = load i32* %i
  %i37 = load i32* %i
  %"int+38" = add i32 %i37, 1
  store i32 %"int+38", i32* %i
  %i39 = load i32* %i
  %i40 = load i32* %i
  %"int+41" = add i32 %i40, 1
  br label %test46

test2:                                            ; No predecessors!
  br label %test44

test3:                                            ; No predecessors!
  %sum48 = load i32* %sum
  ret i32 %sum48

test4:                                            ; No predecessors!

footer:                                           ; preds = %test1

body:                                             ; preds = %test1

test8:                                            ; No predecessors!

body9:                                            ; No predecessors!

body10:                                           ; No predecessors!

footer45:                                         ; No predecessors!

body42:                                           ; No predecessors!

body43:                                           ; No predecessors!

test44:                                           ; preds = %test2

test46:                                           ; preds = %test

footer47:                                         ; No predecessors!
}
