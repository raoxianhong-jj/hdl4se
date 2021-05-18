/* main.v */
module counter
    #(parameter WIDTH=4, MAXVALUE=9, RESETVALUE=0)
(input wClk, nwReset, wCounterIt, 
output [WIDTH-1:0] bCouter, 
output wCounterOverflow);

/*WIDTH��ȵļĴ������������������ֵ*/
reg [WIDTH-1:0] bCurrentCounter;
/*����һ���Ĵ�������ʾ�������Ƿ����*/
reg wOverflow;
wire [WIDTH-1:0] bCounter;
wire wCounterOverflow;
/*�������ֱ�������ڼĴ�����*/
assign bCounter = bCurrentCounter;
assign wCounterOverflow = wOverflow;
always @(posedge wClk) begin
  if (~nwReset) begin /*��λ����*/
  		bCurrentCounter <= RESETVALUE;
        wOverflow <= 1��b0;
  end else begin
    /*��λ�ź���Ч���������ʼ�������� */
	if (wCounterIt) begin
		if (bCurrentCounter == MAXVALUE) begin
			bCurrentCounter <= RESETVALUE;
            wOverflow <= 1��b1;
        end else begin
          bCurrentCounter <= bCurrentCounter + 1;
          wOverflow <= 1��b0;
        end
    end /*wCounterIt*/
  end /*nwReset*/
end /*always*/
endmodule

module dec2seg(input [3:0] dec, output [7:0] seg);
wire [3:0] dec;
reg [7:0] seg;
always @(dec) 
  case (dec)
    4'd0:seg = 8'b00111111;
    4'd1:seg = 8'b00000110;
    4'd2:seg = 8'b01011011;
    4'd3:seg = 8'b01001111;
    4'd4:seg = 8'b01100110;
    4'd5:seg = 8'b01101101;
    4'd6:seg = 8'b01111101;
    4'd7:seg = 8'b00000111;
    4'd8:seg = 8'b01111111;
    4'd9:seg = 8'b01101111;
    default:seg = 8'b01111001;
  endcase
endmodule  

module main(wClk, nwReset, 
            wWrite, bWriteAddr, bWiteData, bWriteMask, 
            wRead, bReadAddr, bReadData);
input wClk, nwReset;
output wWrite;
output [31:0] bWriteAddr;
output [31:0] bWriteData;
output [3:0]  bWriteMask;
output wRead;
output [31:0] bReadAddr;
input [31:0]  bReadData;

wire [31:0] bReadAddr;
wire [31:0]bReadData;
wire wRead;
wire wButton0Pressed;
wire wButton1Pressed;
wire wButton2Pressed;
/*����һֱ�ڶ�������״̬*/
assign wRead = 1��b1;
assign bReadAddr = 32��hF000_0000;
assign wButton0Pressed = bReadData[0];
assign wButton1Pressed = bReadData[1];
assign wButton2Pressed = bReadData[2];

/* �����Ǽ��������� */
assign wCounterin0 = wCounterIt;
wire wCountin0, wCountin1, wCountin2, 
     wCountin3, wCountin4, wCountin5, 
     wCountin6, wCountin7, wCountin8, 
     wCountin9;
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
counter counter8(wClk, nwCounterReset, wCounterin8, bCounter8, wCounterin9);
counter #(RESETVALUE=0, WIDTH=4) count9(.wClk(wClk), .nwReset(nwCounterReset), 
.wCounteit(wCounterin9), .bCounter(bCount9), 
.wConteroverflow());

reg wCounterIt;
/*
����ļĴ�����ָʾ�Ƿ�λ������ֵ, 
����һ���͵�ƽ��Ч���ź�
*/
reg nwResetCount;

always @* begin
  if (~nwReset) begin
nwResetCount = 1��b0;
  end else begin
if (wButton0Pressed)
  nwResetCount = 1��b0;
else
  nwResetCount = 1��b1;
  end
end

/*����Ĵ���������wCounterIt */
always @(posedge wClk) begin
/* ������һ��ʼ�ǲ������ģ�����
������0����ʱ�Լ�������ֵ������
�㣬����1����ʱֹͣ����������2
������ʼ��������ʼ����ʱ����ֵ
�ӵ�ǰֵ��ʼ(��������ͬʱ��
�£��������С��Ϊ׼) 
*/
  if (~nwReset) begin
    wCounterIt <= 1��b0;
  end else if (wButton0Pressed==1��b0) begin
    if (wButton1Pressed) begin
      wCounterIt <= 1��b0;
    end else if (wButton2Pressed) begin
      wCounterIt <= 1��b1;
    end
  end
end 

/* ���������������ӣ�ʮ���������������Ӧ��ʮ�������� */
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
/*���潫���������д������ȥ�������������ʾ*/
/*
�����üĴ��������
ע�⵽����һ��ֻ�����4���ֽڣ����һ��
ʱ���������ֻ�ܿ����ĸ�����ܣ����Ƿ�����
��д������д�仯���ģ��ö�Ӧ������������
��־���õ��Ƿ�仯������Ҫע������������
��һ�ĳ��������Ա仯���Ҳ�Ĵ�һ�ġ�
*/
reg [2:0] bCounterChanged;
always @(posedge wClk)
if (~nwReset)
  bCounterChanged<= 3'b0;
else
  bCounterChanged <= {
  	wCounterin9 | wCounterin8,
  	wCounterin7 | wCounterin6 |	wCounterin5 | wCounterin4,
  	wCounterin3 | wCounterin2 |	wCounterin1 | wCounterin0
  };
  
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
 bWriteMask <= 4'b0;
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
