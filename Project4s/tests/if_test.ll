; ModuleID = 'if_test.bc'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-redhat-linux-gnu"

@a = global float 0.000000e+00

define float @iftest() {
iftest:
  %f = alloca float
  %a = load float* @a
  %a1 = load float* @a
  %0 = fcmp ogt float %a1, 1.000000e+00
  br i1 %0, label %then, label %else

then2:                                            ; No predecessors!
  store float 2.000000e+00, float* %f
  br label %if_footer6

then:                                             ; preds = %iftest
  store float 1.000000e+00, float* %f
  br label %if_footer7

then3:                                            ; No predecessors!
  br label %if_footer

else:                                             ; preds = %iftest
  %f10 = load float* %f
  ret float %f10

if_footer8:                                       ; No predecessors!

then4:                                            ; No predecessors!

then5:                                            ; No predecessors!

if_footer:                                        ; preds = %then3

if_footer6:                                       ; preds = %then2

if_footer7:                                       ; preds = %then

if_footer9:                                       ; No predecessors!
}
