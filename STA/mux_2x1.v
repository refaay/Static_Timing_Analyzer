module mux_2x1 ( A, B, sel, clk, Y );

input A;
input B;
input sel;
input clk;

output Y;

wire N3;

//start cells
DFFX1_RVT Y_reg ( .D(N3), .CLK(clk), .Q(Y) );
MUX21X1_RVT U5 ( .A1(A), .A2(B), .S0(sel), .Y(N3) );

endmodule

