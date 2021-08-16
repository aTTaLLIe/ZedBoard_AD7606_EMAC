
`timescale 1 ns / 1 ps

module AC7606_v1_0_M00_AXIS #
(
    parameter integer C_M_AXIS_TDATA_WIDTH	= 32,
    parameter integer C_M_START_COUNT	= 32,
    parameter WAIT_TRIGGER = 2'b00,
    parameter START = 2'b01,
    parameter WAIT_FIRST_DATA = 2'b10,
    parameter READ_CHANALS = 2'b11
)
(
    // Users to add ports here
    input TRIGGER,
    
    input [15:0]DB,
    input BUSY,
    input FRSTDATA,
    
    output CS,
    output SCLK,
    output CNVST,
    
    output led_0,
    // User ports ends
    // Do not modify the ports beyond this line

    // Global ports
    input wire  M_AXIS_ACLK,
    // 
    input wire  M_AXIS_ARESETN,
    // Master Stream Ports. TVALID indicates that the master is driving a valid transfer, A transfer takes place when both TVALID and TREADY are asserted. 
    output wire  M_AXIS_TVALID,
    // TDATA is the primary payload that is used to provide the data that is passing across the interface from the master.
    output wire [C_M_AXIS_TDATA_WIDTH-1 : 0] M_AXIS_TDATA,
    // TSTRB is the byte qualifier that indicates whether the content of the associated byte of TDATA is processed as a data byte or a position byte.
    output wire [(C_M_AXIS_TDATA_WIDTH/8)-1 : 0] M_AXIS_TSTRB,
    // TLAST indicates the boundary of a packet.
    output wire  M_AXIS_TLAST,
    // TREADY indicates that the slave can accept a transfer in the current cycle.
    input wire  M_AXIS_TREADY
);

wire reset;

(* MARK_DEBUG="true" *) reg start_processing = 1'b0;

reg [1:0] state = WAIT_TRIGGER;
reg CS_reg = 1'b1;
reg SCLK_reg = 1'b1;
reg CNVST_reg = 1'b1;
reg [15:0] ADC_data_reg = 1'b0;

reg BUSY_now = 1'b0;
reg BUSY_past = 1'b0;
reg [3:0] count_chanal = 1'b0;
reg [1:0] read_ch = 1'b0;

wire clk_100MHz;
wire clk_50MHz;

wire m_axis_tvalid_in;     
reg m_axis_tready_in = 1;     
wire [31:0] m_axis_tdata_in;              

reg s_axis_volt_tvalid = 0;
reg [31:0] s_axis_volt_tdata = 0;
wire s_axis_volt_tready;
wire m_axis_volt_tvalid;       
wire m_axis_volt_tready;        
wire [31 : 0] m_axis_volt_tdata;

reg s_axis_const_tvalid = 0;       
wire s_axis_const_tready;      
reg [31 : 0] s_axis_const_tdata = 0;

wire m_axis_result_tvalid;
wire [31:0] m_axis_result_tdata;  
reg s_axis_result_tlast = 0;

reg s_axis_result_tvalid_out = 0;
wire s_axis_result_tready_out;
reg [31:0] s_axis_result_tdata_out = 0;
reg s_axis_result_tlast_out = 0;

wire m_axis_result_const_tvalid;       
wire m_axis_result_const_tready;        
wire [31 : 0] m_axis_result_const_tdata;

reg [31:0] const_positive = 32'h391F3CB0;
reg [31:0] const_negative = 32'hB91F3CB0;
reg [8:0] count_packet = 0;

clk_wiz clk_block
(
.clk_out1(clk_100MHz),     
.clk_out2(clk_50MHz),     
.reset(reset),
.clk_in1(M_AXIS_ACLK)
);      

always @(posedge M_AXIS_ACLK) begin
    if(reset)begin
        start_processing <= 1'b0; 
    end
    else begin
        if(TRIGGER)begin
            start_processing <= 1'b1;
        end
        else begin
            if(state == READ_CHANALS)begin
                start_processing <= 1'b0;
            end
        end
    end
end

reg flag_first = 1'b0;
always @(posedge clk_50MHz)begin
    if(reset)begin
        state <= WAIT_TRIGGER;
        CNVST_reg <= 1'b1;
        BUSY_now <= 1'b0;
        BUSY_past <= 1'b0;
        CS_reg <= 1'b1;
        SCLK_reg <= 1'b1;
        count_chanal <= 1'b0;
        read_ch <= 1'b0;
    end
    else begin
        case(state)
            WAIT_TRIGGER:begin
                if(start_processing == 1'b1)begin
                    state <= START;
                    CNVST_reg <= 1'b0;
                end
            end
            START:begin
                CNVST_reg <= 1'b1;
                BUSY_now <= BUSY;
                BUSY_past <= BUSY_now;
                if(BUSY_now == 1'b0 && BUSY_past == 1'b1)begin
                    state <= WAIT_FIRST_DATA;
                    BUSY_now <= 1'b0;
                    BUSY_past <= 1'b0;
                end             
            end
            WAIT_FIRST_DATA:begin
                if(FRSTDATA)begin
                    if(flag_first == 1'b0)begin
                        flag_first <= 1'b1;
                        CS_reg <= 1'b0;
                        SCLK_reg <= 1'b0;
                    end 
                    else begin
                        state <= READ_CHANALS;
                        CS_reg <= 1'b1;
                        SCLK_reg <= 1'b1;
                    end
                end
            end
            READ_CHANALS:begin
                CS_reg <= 1'b0;
                flag_first <= 1'b0;
                if(count_chanal != 4'd8) begin
                    if(read_ch == 1'b0)begin
                        ADC_data_reg <= DB;
                        SCLK_reg <= 1'b0;
                        read_ch <= 1'b1;
                        if(count_chanal == 1'b0 && s_axis_volt_tready && s_axis_const_tready)begin
                            if(DB[15] == 0)begin
                                s_axis_volt_tvalid <= 1'b1;
                                s_axis_volt_tdata <= {16'h00, DB};
                                s_axis_const_tvalid <= 1'b1;
                                s_axis_const_tdata <= const_positive;
                            end
                            else begin
                                s_axis_volt_tvalid <= 1'b1;
                                s_axis_volt_tdata <= {16'h00, DB^16'hFFFF};
                                s_axis_const_tvalid <= 1'b1;
                                s_axis_const_tdata <= const_negative;
                            end
                        end
                    end
                    else begin
                            s_axis_volt_tvalid <= 1'b0;
                            s_axis_const_tvalid <= 1'b0;
                            SCLK_reg <= 1'b1;
                            read_ch <= 1'b0;
                            count_chanal <= count_chanal + 1'b1;
                    end
                end
                else begin
                    CS_reg <= 1'b1;
                    count_chanal <= 1'b0;
                    state <= WAIT_TRIGGER; 
                end 
            end
            default:begin
                state <= WAIT_TRIGGER;
            end   
        endcase
    end
end

always @(posedge clk_50MHz)begin
    if(reset)begin
        s_axis_result_tvalid_out <= 1'b0;
        s_axis_result_tdata_out <= 32'h00000000;
        count_packet <= 9'h00;
    end 
    else begin
        if(s_axis_result_tready_out && m_axis_result_tvalid)begin
            s_axis_result_tvalid_out <= 1'b1;
            s_axis_result_tdata_out <= m_axis_result_tdata;
            if(~TRIGGER)begin
                s_axis_result_tlast_out <= 1'b1;
                count_packet <= 9'h00;
            end
            else begin
                if(count_packet >= 9'd300)begin
                    s_axis_result_tlast_out <= 1'b1;
                    count_packet <= 9'h00;
                end
                else count_packet <= count_packet + 9'h01;
            end
        end
        else begin
            s_axis_result_tvalid_out <= 1'b0;
        end
        
        if(s_axis_result_tlast_out) s_axis_result_tlast_out <= 1'b0;
    end   
end


axis_data_fifo fifo_output (
  .wr_rst_busy(),      // output wire wr_rst_busy
  .rd_rst_busy(),      // output wire rd_rst_busy
  .m_aclk(M_AXIS_ACLK),              // input wire m_aclk
  .s_aclk(clk_50MHz),                // input wire s_aclk
  .s_aresetn(~reset),         // input wire s_aresetn
  .s_axis_tvalid(s_axis_result_tvalid_out),  // input wire s_axis_tvalid
  .s_axis_tready(s_axis_result_tready_out),  // output wire s_axis_tready
  .s_axis_tdata(s_axis_result_tdata_out),    // input wire [31 : 0] s_axis_tdata
  .s_axis_tlast(s_axis_result_tlast_out),    // input wire s_axis_tlast
  .m_axis_tvalid(M_AXIS_TVALID),  // output wire m_axis_tvalid
  .m_axis_tready(M_AXIS_TREADY),  // input wire m_axis_tready
  .m_axis_tdata(M_AXIS_TDATA),    // output wire [31 : 0] m_axis_tdata
  .m_axis_tlast(M_AXIS_TLAST)    // output wire m_axis_tlast
);


fix_to_float fix_to_float (
  .aclk(clk_50MHz),                                  // input wire aclk
  .s_axis_a_tvalid(s_axis_volt_tvalid),            // input wire s_axis_a_tvalid
  .s_axis_a_tready(s_axis_volt_tready),            // output wire s_axis_a_tready
  .s_axis_a_tdata(s_axis_volt_tdata),              // input wire [31 : 0] s_axis_a_tdata
  .m_axis_result_tvalid(m_axis_volt_tvalid),  // output wire m_axis_result_tvalid
  .m_axis_result_tready(m_axis_volt_tready),  // input wire m_axis_result_tready
  .m_axis_result_tdata(m_axis_volt_tdata)    // output wire [31 : 0] m_axis_result_tdata
);

float_float float_float (
  .aclk(clk_50MHz),                                  // input wire aclk
  .s_axis_a_tvalid(s_axis_const_tvalid),            // input wire s_axis_a_tvalid
  .s_axis_a_tready(s_axis_const_tready),            // output wire s_axis_a_tready
  .s_axis_a_tdata(s_axis_const_tdata),              // input wire [31 : 0] s_axis_a_tdata
  .m_axis_result_tvalid(m_axis_result_const_tvalid),  // output wire m_axis_result_tvalid
  .m_axis_result_tready(m_axis_result_const_tready),  // input wire m_axis_result_tready
  .m_axis_result_tdata(m_axis_result_const_tdata)    // output wire [31 : 0] m_axis_result_tdata
);

float_divide float_divide (
  .aclk(clk_50MHz),                                  // input wire aclk
  .s_axis_a_tvalid(m_axis_volt_tvalid),            // input wire s_axis_a_tvalid
  .s_axis_a_tready(m_axis_volt_tready),            // output wire s_axis_a_tready
  .s_axis_a_tdata(m_axis_volt_tdata),              // input wire [31 : 0] s_axis_a_tdata
  .s_axis_b_tvalid(m_axis_result_const_tvalid),            // input wire s_axis_b_tvalid
  .s_axis_b_tready(m_axis_result_const_tready),            // output wire s_axis_b_tready
  .s_axis_b_tdata(m_axis_result_const_tdata),              // input wire [31 : 0] s_axis_b_tdata
  .m_axis_result_tvalid(m_axis_result_tvalid),  // output wire m_axis_result_tvalid
  .m_axis_result_tready(s_axis_result_tready_out),  // input wire m_axis_result_tready
  .m_axis_result_tdata(m_axis_result_tdata)    // output wire [31 : 0] m_axis_result_tdata
);


assign led_0 = start_processing;
assign CS = CS_reg;
assign SCLK = SCLK_reg;
assign CNVST = CNVST_reg;

assign reset = ~M_AXIS_ARESETN;
endmodule
