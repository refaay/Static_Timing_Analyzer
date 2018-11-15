module VH ( A, clk, Y);

input A;
input clk;

output Y;

wire N1;
wire N2;
wire N3;

//start cells
DFF_X80 FF1 ( .d(A), .ck(clk), .q(N1) );
DFF_X80 FF2 ( .d(N2), .ck(N3), .q(Y) );
BUF_X1 BUFF1 ( .a(clk), .o(N3) );
INV_X2 INV1 ( .o(N2), .a(N1) );

endmodule
