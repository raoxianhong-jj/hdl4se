(* 
  HDL4SE="LCOM", 
  CLSID="9B0B3D25-346D-48B9-ABB9-ED755910425D", 
  library="hdl4se" 
*) 
module #(WIDTH=8) 
  hdl4se_mux2
  (
    input  sel,
    input [WIDTH-1:0] in0,
    input [WIDTH-1:0] in1,
    output [WIDTH-1:0] data
  );
   reg [WIDTH-1:0] data;
   wire sel;
   wire [WIDTH-1:0] in0, in1;
   always @*
     case (sel)
       1'b0: data = in0;
       1'b1: data = in1;
     endcase
endmodule

(* 
  HDL4SE="LCOM", 
  CLSID="041F3AA1-97CD-4412-9E8E-D04ADF291AE2", 
  library="hdl4se" 
*) 
module #(WIDTH=8) 
  hdl4se_mux4
  (
    input [1:0] sel,
    input [WIDTH-1:0] in0,
    input [WIDTH-1:0] in1,
    input [WIDTH-1:0] in2,
    input [WIDTH-1:0] in3,
    output [WIDTH-1:0] data
  );
   reg [WIDTH-1:0] data;
   wire [1:0] sel;
   wire [WIDTH-1:0] in0, in1, in2, in3;
   always @*
     case (sel)
       2'd0: data = in0;
       2'd1: data = in1;
       2'd2: data = in2;
       2'd3: data = in3;
     endcase
endmodule

(* 
  HDL4SE="LCOM", 
  CLSID="DD99B7F6-9ED1-45BB-8150-ED78EEF982CA", 
  library="hdl4se" 
*) 
module #(WIDTH=8) 
  hdl4se_mux8
  (
    input [2:0] sel,
    input [WIDTH-1:0] in0,
    input [WIDTH-1:0] in1,
    input [WIDTH-1:0] in2,
    input [WIDTH-1:0] in3,
    input [WIDTH-1:0] in4,
    input [WIDTH-1:0] in5,
    input [WIDTH-1:0] in6,
    input [WIDTH-1:0] in7,
    output [WIDTH-1:0] data
  );
   reg [WIDTH-1:0] data;
   wire [2:0] sel;
   wire [WIDTH-1:0] in0, in1, in2, in3, in4, in5, in6, in7;
   always @*
     case (sel)
       3'd0: data = in0;
       3'd1: data = in1;
       3'd2: data = in2;
       3'd3: data = in3;
       3'd4: data = in4;
       3'd5: data = in5;
       3'd6: data = in6;
       3'd7: data = in7;
     endcase
endmodule

(* 
  HDL4SE="LCOM", 
  CLSID="69B4A095-0644-4B9E-9CF0-295474D7C243", 
  library="hdl4se" 
*) 
module #(WIDTH=8) 
  hdl4se_mux16
  (
    input [3:0] sel,
    input [WIDTH-1:0] in0,
    input [WIDTH-1:0] in1,
    input [WIDTH-1:0] in2,
    input [WIDTH-1:0] in3,
    input [WIDTH-1:0] in4,
    input [WIDTH-1:0] in5,
    input [WIDTH-1:0] in6,
    input [WIDTH-1:0] in7,
    input [WIDTH-1:0] in8,
    input [WIDTH-1:0] in9,
    input [WIDTH-1:0] in10,
    input [WIDTH-1:0] in11,
    input [WIDTH-1:0] in12,
    input [WIDTH-1:0] in13,
    input [WIDTH-1:0] in14,
    input [WIDTH-1:0] in15,
    output [WIDTH-1:0] data
  );
   reg [WIDTH-1:0] data;
   wire [3:0] sel;
   wire [WIDTH-1:0] in0, in1, in2, in3, in4, in5, in6, in7,
                    in8, in9, in10, in11, in12, in13, in14, in15;
   always @*
     case (sel)
       4'd0: data = in0;
       4'd1: data = in1;
       4'd2: data = in2;
       4'd3: data = in3;
       4'd4: data = in4;
       4'd5: data = in5;
       4'd6: data = in6;
       4'd7: data = in7;
       4'd8: data = in8;
       4'd9: data = in9;
       4'd10: data = in10;
       4'd11: data = in11;
       4'd12: data = in12;
       4'd13: data = in13;
       4'd14: data = in14;
       4'd15: data = in15;
     endcase
endmodule

(* 
   HDL4SE="LCOM", 
   CLSID="29D9C8D6-810E-41D0-BCEF-A5B86EE1EE01",
   library="hdl4se" 
*) 
module #(INPUTWIDTH=16, 
         OUTPUTWIDTH0=8, OUTPUTFROM0=0, 
         OUTPUTWIDTH1=8, OUTPUTFROM1=8) 
  hdl4se_split2(
      input [INPUTWIDTH-1:0] wirein
      output [OUTPUTWIDTH0-1:0] wireout0,
      output [OUTPUTWIDTH1-1:0] wireout1
    );
  wire [INPUTWIDTH-1:0] wirein;
  wire [OUTPUTWIDTH0-1:0] wireout0;
  wire [OUTPUTWIDTH1-1:0] wireout1;
  assign wireout0 = wirein[OUTPUTWIDTH0+OUTPUTFROM0-1:OUTPUTFROM0];
  assign wireout1 = wirein[OUTPUTWIDTH1+OUTPUTFROM1-1:OUTPUTFROM1];
endmodule

(* 
   HDL4SE="LCOM", 
   CLSID="D5152459-6798-49C8-8376-21EBE8A9EE3C",
   library="hdl4se" 
*) 
module #(INPUTWIDTH=32, 
         OUTPUTWIDTH0=8, OUTPUTFROM0=0, 
         OUTPUTWIDTH1=8, OUTPUTFROM1=8
         OUTPUTWIDTH2=8, OUTPUTFROM2=16, 
         OUTPUTWIDTH3=8, OUTPUTFROM3=24
         ) 
  hdl4se_split4(
      input [INPUTWIDTH-1:0] wirein
      output [OUTPUTWIDTH0-1:0] wireout0,
      output [OUTPUTWIDTH1-1:0] wireout1,
      output [OUTPUTWIDTH2-1:0] wireout2,
      output [OUTPUTWIDTH3-1:0] wireout3,
    );
  wire [INPUTWIDTH-1:0] wirein;
  wire [OUTPUTWIDTH0-1:0] wireout0;
  wire [OUTPUTWIDTH1-1:0] wireout1;
  wire [OUTPUTWIDTH2-1:0] wireout2;
  wire [OUTPUTWIDTH3-1:0] wireout3;
  assign wireout0 = wirein[OUTPUTWIDTH0+OUTPUTFROM0-1:OUTPUTFROM0];
  assign wireout1 = wirein[OUTPUTWIDTH1+OUTPUTFROM1-1:OUTPUTFROM1];
  assign wireout2 = wirein[OUTPUTWIDTH2+OUTPUTFROM2-1:OUTPUTFROM2];
  assign wireout3 = wirein[OUTPUTWIDTH3+OUTPUTFROM3-1:OUTPUTFROM3];
endmodule

(* 
   HDL4SE="LCOM", 
   CLSID="DA8C1494-B6F6-4910-BB2B-C9BCFCB9FAD0",
   library="hdl4se" 
*) 
module #(
         INPUTWIDTH0=8, 
         INPUTWIDTH1=8
         ) 
  hdl4se_bind2(
      input [INPUTWIDTH0-1:0] wirein0,
      input [INPUTWIDTH1-1:0] wirein1,
      output [INPUTWIDTH0+INPUTWIDTH1-1:0] wireout
    );
  wire [INPUTWIDTH0-1:0] wirein0;
  wire [INPUTWIDTH1-1:0] wirein1;
  wire [INPUTWIDTH0+INPUTWIDTH1-1:0] wireout;
  assign wireout = {wirein1, wirein0};
endmodule

(* 
   HDL4SE="LCOM", 
   CLSID="D1F303E2-3ED1-42FD-8762-3AA623DA901E",
   library="hdl4se" 
*) 
module #(
         INPUTWIDTH0=8, 
         INPUTWIDTH1=8, 
         INPUTWIDTH2=8
         ) 
  hdl4se_bind3(
      input [INPUTWIDTH0-1:0] wirein0,
      input [INPUTWIDTH1-1:0] wirein0,
      input [INPUTWIDTH2-1:0] wirein1,
      output [INPUTWIDTH0+INPUTWIDTH1+INPUTWIDTH2-1:0] wireout
    );
  wire [INPUTWIDTH0-1:0] wirein0;
  wire [INPUTWIDTH1-1:0] wirein1;
  wire [INPUTWIDTH2-1:0] wirein2;
  wire [INPUTWIDTH0+INPUTWIDTH1+INPUTWIDTH2-1:0] wireout;
  assign wireout = {wirein2, wirein1, wirein0};
endmodule

(* 
   HDL4SE="LCOM", 
   CLSID="0234ECE7-A9C5-406B-9AE7-4841EA0DF7C9",
   library="hdl4se" 
*) 
module #(
         INPUTWIDTH0=8, 
         INPUTWIDTH1=8, 
         INPUTWIDTH2=8, 
         INPUTWIDTH3=8
         ) 
  hdl4se_bind4(
      input [INPUTWIDTH0-1:0] wirein0,
      input [INPUTWIDTH1-1:0] wirein1,
      input [INPUTWIDTH2-1:0] wirein2,
      input [INPUTWIDTH3-1:0] wirein3,
      output [INPUTWIDTH0+INPUTWIDTH1+INPUTWIDTH2+INPUTWIDTH3-1:0] wireout
    );
  wire [INPUTWIDTH0-1:0] wirein0;
  wire [INPUTWIDTH1-1:0] wirein1;
  wire [INPUTWIDTH2-1:0] wirein2;
  wire [INPUTWIDTH3-1:0] wirein3;
  wire [INPUTWIDTH0+INPUTWIDTH1+INPUTWIDTH2+INPUTWIDTH3-1:0] wireout;
  assign wireout = {wirein3, wirein2, wirein1, wirein0};
endmodule

(* 
   HDL4SE="LCOM", 
   CLSID="8FBE5B87-B484-4f95-8291-DBEF86A1C354",
   library="hdl4se" 
*) 
module #(VALUEWIDTH=8, VALUE=8'b0) 
  hdl4se_const(output [VALUEWIDTH-1:0] data);
  wire [VALUEWIDTH-1:0] data;
  assign data = VALUE;
endmodule

`define BINOP_ADD 0
`define BINOP_SUB 1
`define BINOP_MUL 2
`define BINOP_DIV 3
`define BINOP_EQ 4 
`define BINOP_NEQ 5
`define BINOP_LT 6
`define BINOP_LE 7
`define BINOP_GE 8
`define BINOP_GT 9
`define BINOP_AND 10
`define BINOP_OR 11
`define BINOP_XOR 12
(* 
   HDL4SE="LCOM", 
   CLSID="060FB913-1C0F-4704-8EC2-A08BF5387062",
   library="hdl4se" 
*) 
module #(INPUTWIDTH0=8, INPUTWIDTH1=8, OUTPUTWIDTH=8, OP=`BINOP_ADD) 
  hdl4se_binop(
      input [INPUTWIDTH0-1:0] wirein0,
      input [INPUTWIDTH1-1:0] wirein1,
      output [OUTPUTWIDTH-1:0] wireout
    );
  wire [INPUTWIDTH0-1:0] wirein0;
  wire [INPUTWIDTH1-1:0] wirein1;
  wire [OUTPUTWIDTH-1:0] wireout;
  
endmodule

`define UNOP_NEG 0
`define UNOP_BITAND 1
`define UNOP_BITOR 2
`define UNOP_BITXOR 3
(* 
   HDL4SE="LCOM", 
   CLSID="E6772805-57BB-4b39-A10D-FDA6A4810E3B",
   library="hdl4se" 
*) 
module #(INPUTWIDTH=8, OUTPUTWIDTH=8, OP=`UNOP_NEG) 
  hdl4se_unop(
      input [INPUTWIDTH-1:0] wirein,
      output [OUTPUTWIDTH-1:0] wireout
    );
  wire [INPUTWIDTH-1:0] wirein;
  wire [OUTPUTWIDTH-1:0] wireout;
endmodule

(* 
   HDL4SE="LCOM", 
   CLSID="76FBFD4B-FEAD-45fd-AA27-AFC58AC241C2",
   library="hdl4se" 
*) 
module #(WIDTH=8) 
  hdl4se_reg(
      input wClk,
      input [WIDTH-1:0] wirein,
      output [WIDTH-1:0] wireout
    );
  wire [WIDTH-1:0] wirein;
  reg [WIDTH-1:0] wireout;
  always @(posedge wClk) wireout <= wirein;
endmodule
