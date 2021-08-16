`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 25.07.2021 10:36:05
// Design Name: 
// Module Name: test_bench
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module test_bench(
    );
    
reg [15:0]DB;
reg BUSY;
reg FRSTDATA;

wire CS;
wire SCLK;
wire CNVST;

wire led_0;
reg  s00_axis_aclk;
reg  s00_axis_aresetn;
wire  s00_axis_tready;
reg [31 : 0] s00_axis_tdata;
reg [3: 0] s00_axis_tstrb;
reg  s00_axis_tlast;
reg  s00_axis_tvalid;

reg  m00_axis_aclk;
reg  m00_axis_aresetn;
wire  m00_axis_tvalid;
wire [31: 0] m00_axis_tdata;
wire [3 : 0] m00_axis_tstrb;
wire  m00_axis_tlast;
reg  m00_axis_tready;  
    
    
    
    
AC7606_v1_0#
()
test_fun( 
    .DB(DB),
    .BUSY(BUSY),
    .FRSTDATA(FRSTDATA),
    
    .CS(CS),
    .SCLK(SCLK),
    .CNVST(CNVST),
    
    .led_0(led_0),
    
    .s00_axis_aclk(s00_axis_aclk),
    .s00_axis_aresetn(s00_axis_aresetn),
    .s00_axis_tready(s00_axis_tready),
    .s00_axis_tdata(s00_axis_tdata),
    .s00_axis_tstrb(s00_axis_tstrb),
    .s00_axis_tlast(s00_axis_tlast),
    .s00_axis_tvalid(s00_axis_tvalid),
    

    .m00_axis_aclk(m00_axis_aclk),
    .m00_axis_aresetn(m00_axis_aresetn),
    .m00_axis_tvalid(m00_axis_tvalid),
    .m00_axis_tdata(m00_axis_tdata),
    .m00_axis_tstrb(m00_axis_tstrb),
    .m00_axis_tlast(m00_axis_tlast),
    .m00_axis_tready(m00_axis_tready)
);
endmodule
