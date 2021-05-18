
module dec2seg(input [3:0] dec, output [7:0] seg);
  wire [7:0] wire_cst0;
  hdl4se_const #(8, 8'b00111111) const_cst0(wire_cst0);
  wire [7:0] wire_cst1;
  hdl4se_const #(8, 8'b00000110) const_cst1(wire_cst1);
  wire [7:0] wire_cst2;
  hdl4se_const #(8, 8'b01011011) const_cst2(wire_cst2);
  wire [7:0] wire_cst3;
  hdl4se_const #(8, 8'b01001111) const_cst3(wire_cst3);
  wire [7:0] wire_cst4;
  hdl4se_const #(8, 8'b01100110) const_cst4(wire_cst4);
  wire [7:0] wire_cst5;
  hdl4se_const #(8, 8'b01101101) const_cst5(wire_cst5);
  wire [7:0] wire_cst6;
  hdl4se_const #(8, 8'b01111101) const_cst6(wire_cst6);
  wire [7:0] wire_cst7;
  hdl4se_const #(8, 8'b00000111) const_cst7(wire_cst7);
  wire [7:0] wire_cst8;
  hdl4se_const #(8, 8'b01111111) const_cst8(wire_cst8);
  wire [7:0] wire_cst9;
  hdl4se_const #(8, 8'b01101111) const_cst9(wire_cst9);
  wire [7:0] wire_cst10;
  hdl4se_const #(8, 8'b01111001) const_cst10(wire_cst10);

  hdl4se_mux16 #(8) mux_dec(dec, 
    wire_cst0, 
    wire_cst1, 
    wire_cst2, 
    wire_cst3, 
    wire_cst4, 
    wire_cst5, 
    wire_cst6, 
    wire_cst7, 
    wire_cst8, 
    wire_cst9, 
    wire_cst10, 
    wire_cst10, 
    wire_cst10, 
    wire_cst10, 
    wire_cst10, 
    wire_cst10, 
    wire_cst10 
    seg);
endmodule  

module counter
    #(parameter WIDTH=4, MAXVALUE=9, RESETVALUE=0)
(input wClk, nwReset, wCounterIt, 
output [WIDTH-1:0] bCouter, 
output wCounterOverflow);

/*WIDTH宽度的寄存器用来保存计数器的值*/
  wire [WIDTH-1:0] wirein_bCurrentCounter, wireout_bCurrentCounter;
  wire wEn_bCurrentCounter;
  hdl4se_reg #(WIDTH, RESETVALUE) bCurrentCounter(
      wClk, nwReset, wEn_bCurrentCounter,
      wirein_bCurrentCounter, wireout_bCurrentCounter
    );
/*定义一个寄存器来表示计数器是否溢出*/
  wire wirein_wOverflow, wireout_wOverflow;
  wire wEn_wOverflow;
  hdl4se_reg #(1, 0) reg_wOverflow(
      wClk, nwReset, wEn_wOverflow,
      wirein_wOverflow, wireout_wOverflow
    );
wire [WIDTH-1:0] bCounter;
wire wCounterOverflow;
assign bCounter = wireout_bCurrentCounter;
assign wCounterOverflow = wireout_wOverflow;
assign wEn_bCurrentCounter = wCounterIt;
assign wEn_wOverflow= wCounterIt;
wire [WIDTH-1:0] bConst_MAXVALUE;
/*常数 MAXVALUE*/
hdl4se_const #(WIDTH, MAXVALUE;
    ) const_MAXVALUE(bConst_MAXVALUE);
/*常数 RESETVALUE*/
wire [WIDTH-1:0] bConst_RESETVALUE;
hdl4se_const #(WIDTH, RESETVALUE;
    ) const_RESETVALUE(bConst_RESETVALUE);    
wire wEQ_bCurrentCounter_MAXVALUE;
/* 比较器 bCurrentCounter == MAXVALUE */
hdl4se_binop #(WIDTH, WIDTH, 1, BINOP_CMP_EQ) binop_EQ_bCurrentCounter_MAXVALUE(
      wireout_bCurrentCounter,
      bConst_MAXVALUE,
      wEQ_bCurrentCounter_MAXVALUE
    );
/* bCurrentCounter+1 用加法器实现 */    
/*常数 1*/
wire [WIDTH-1:0] bConst_One;
wire [WIDTH-1:0] bCurrentCounterPlusOne;
hdl4se_const #(WIDTH, 1;
    ) const_One(bConst_One);    
hdl4se_binop #(WIDTH, WIDTH, 1, BINOP_ADD) binop_bCurrentCounterInc(
      wireout_bCurrentCounter,
      bConst_One,
      bCurrentCounterPlusOne
    );
/* if语句用数据选择器实现 */
wire [WIDTH*2-1:0] bMuxIn;
hdl4se_binding2to1 #(WIDTH, WIDTH) bind2to1(bCurrentCounterPlusOne, bConst_RESETVALUE,bMuxIn);
hdl4se_mux #(1, 2*WIDTH, WIDTH) muxCurrentCouter(wEQ_bCurrentCounter_MAXVALUE, bMuxIn, wirein_bCurrentCounter);
wire [1:0] bConst_10;
hdl4se_const #(2, 2'b10;
    ) const_10(bConst_10); 
hdl4se_mux #(1, 2, 1) muxOverflow(wEQ_bCurrentCounter_MAXVALUE, bConst_10, wirein_wOverflow);
endmodule

module main(input wClk, 
            input nwReset, 
            output wWrite, 
            output [31:0] bWriteAddr, 
            output [31:0] bWiteData, 
            output [3:0] bWriteMask, 
            output wRead, 
            output [31:0] bReadAddr, 
            input [31:0] bReadData);

  wire wButton0Pressed;
  wire wButton1Pressed;
  wire wButton2Pressed;
  /*我们一直在读按键的状态*/
  assign wRead = 1’b1;
  assign bReadAddr = 32’hF000_0000;
  assign wButton0Pressed = bReadData[0];
  assign wButton1Pressed = bReadData[1];
  assign wButton2Pressed = bReadData[2];

/* 以下是计数器连接 */
  assign wCounterin0 = wCounterIt;
  wire wCountin0, wCountin1, wCountin2, 
     wCountin3, wCountin4, wCountin5, 
     wCountin6, wCountin7, wCountin8, 
     wCountin9, wTemp_0000;
  wire [3:0] bCount0, bCount1, bCount2, bCount3, bCount4,
           bCount5, bCount6, bCount7, bCount8, bCount9;
  counter #(4,9,0) counter0(wClk, nwCounterReset, wCounterin0, bCount0, wCounterin1);
  counter #(4,9,0) counter1(wClk, nwCounterReset, wCounterin1, bCount1, wCounterin2);
  counter #(4,9,0) counter2(wClk, nwCounterReset, wCounterin2, bCount2, wCounterin3);
  counter #(4,9,0) counter3(wClk, nwCounterReset, wCounterin3, bCount3, wCounterin4);
  counter #(4,9,0) counter4(wClk, nwCounterReset, wCounterin4, bCount4, wCounterin5);
  counter #(4,9,0) counter5(wClk, nwCounterReset, wCounterin5, bCount5, wCounterin6);
  counter #(4,9,0) counter6(wClk, nwCounterReset, wCounterin6, bCount6, wCounterin7);
  counter #(4,9,0) counter7(wClk, nwCounterReset, wCounterin7, bCount7, wCounterin8);
  counter #(4,9,0) counter8(wClk, nwCounterReset, wCounterin8, bCount8, wCounterin9);
  counter #(4,9,0) counter9(wClk, nwCounterReset, wCounterin9, bCount9, wTemp_0000);

  wire [WIDTH-1:0] wirein_wCounterIt, wireout_wCounterIt;
  wire wEn_wCounterIt;
  hdl4se_reg #(1, 0) wCounterIt(
      wClk, nwReset, wEn_wCounterIt,
      wirein_wCounterIt, wireout_wCounterIt
    );
  wire wButton0NotPressed;
  hdl4se_unop #(1, 1, UNOP_NOT) Button0NotPressed(wButton0Pressed, wButton0NotPressed); 
  assign wEn_wCounterIt = wButton0NotPressed;

  /*counterit= (~b1) & b2*/
  wire wButton1NotPressed;
  hdl4se_unop #(1, 1, UNOP_NOT) Button1NotPressed(wButton1Pressed, wButton1NotPressed); 
  hdl4se_binop #(1, 1, 1, BINOP_AND) binop_counterit(wButton1NotPressed, wButton2Pressed, wirein_wCounterIt);

  wire nwResetCount;
  /*assign nwResetCount = (~b0) & nwReset;	*/
  hdl4se_binop #(1, 1, 1, BINOP_AND) binop_resetcounter(wButton0NotPressed, nwReset, nwResetCount);

/* 以下是译码器连接，十个计数器的输出对应到十个译码器 */
wire code0[7:0];
wire code1[7:0];
wire code2[7:0];
wire code3[7:0];
wire code4[7:0];
wire code5[7:0];
wire code6[7:0];
wire code7[7:0];
wire code8[7:0];
wire code9[7:0];
dec2seg dec0(bCount0, code0);
dec2seg dec1(bCount1, code1);
dec2seg dec2(bCount2, code2);
dec2seg dec3(bCount3, code3);
dec2seg dec4(bCount4, code4);
dec2seg dec5(bCount5, code5);
dec2seg dec6(bCount6, code6);
dec2seg dec7(bCount7, code7);
dec2seg dec8(bCount8, code8);
dec2seg dec9(bCount9, code9);

/*下面将译码器输出写到外面去，控制数码管显示*/

/*
我们用寄存器输出，
注意到我们一次只能输出4个字节，因此一个
时钟周期最多只能控制四个数码管，我们分三段
来写，优先写变化慢的，用对应计数器的输入
标志来得到是否变化。不过要注意计数器的输出
晚一拍出来，所以变化情况也寄存一拍。
*/
  wire [2:0] wirein_bCounterChanged, wireout_bCounterChanged;
  wire wEn_bCounterChanged;
  hdl4se_reg #(3, 0) wCounterIt(
      wClk, nwReset, wEn_bCounterChanged,
      wirein_bCounterChanged, wireout_bCounterChanged
    );
  hdl4se_const #(1, 1) wConstOne(wEn_bCounterChanged);  
  wire wCounterin98, wCounterin76, wCounterin54, wCounterin32, wCounterin10, 
       wCounterin7654, wCounterin3210;
  hdl4se_binop #(1, 1, 1, BINOP_OR) or98(wCounterin9, wCounterin8, wCounterin98); 
  hdl4se_binop #(1, 1, 1, BINOP_OR) or76(wCounterin7, wCounterin6, wCounterin76); 
  hdl4se_binop #(1, 1, 1, BINOP_OR) or54(wCounterin5, wCounterin4, wCounterin54); 
  hdl4se_binop #(1, 1, 1, BINOP_OR) or32(wCounterin3, wCounterin2, wCounterin32); 
  hdl4se_binop #(1, 1, 1, BINOP_OR) or10(wCounterin1, wCounterin0, wCounterin10); 
  hdl4se_binop #(1, 1, 1, BINOP_OR) or32(wCounterin76, wCounterin54, wCounterin7654); 
  hdl4se_binop #(1, 1, 1, BINOP_OR) or10(wCounterin32, wCounterin10, wCounterin3210); 
  wire [1:0] bChanged70;
  hdl4se_binding2to1 #(1, 1)(wCounterin7654, wCounterin3210, bChanged70);
  hdl4se_binding2to1 #(1, 2)(wCounterin98, bChanged70, wirein_bCounterChanged);

reg wWrite;
reg [31:0] bWriteAddr;
reg [31:0] bWriteData;
reg [3:0] bWriteMask;

always @posedge wClk)
if (~nwReset) begin
  wWrite <= 1'b0;
  bWriteAddr <= 32'b0;
  bWriteData <= 32'b0; 
  bWriteMask <= 4'b0;
end else begin
 wWrite <= 1'b0;
 if (bCounterChanged[2]) begin
   wWrite <= 1'b1;
   bWriteMask <= 4'b1100;
   bWriteAddr <= 32'hf0000018;
   bWriteData <= {16'b0, code9, code8};
 end else if (bCounterChanged[1]) begin
   wWrite <= 1'b1;
   bWriteAddr <= 32'hf0000014;
   bWriteData <= {code7, code6, code5, code4};
 end  else if (bCounterChanged[0]) begin
   wWrite <= 1'b1;
   bWriteAddr <= 32'hf0000010;
   bWriteData <= {code3, code2, code1, code0};
 end
end

endmodule
