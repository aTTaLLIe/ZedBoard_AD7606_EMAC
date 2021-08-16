
`timescale 1 ns / 1 ps

module AC7606_v1_0_S00_AXIS #
(
    // Users to add parameters here
    parameter IDLE = 8'h00,
    parameter PROCESSING = 8'h01,
    parameter END = 8'h02,
    // User parameters ends
    // Do not modify the parameters beyond this line

    // AXI4Stream sink: Data Width
    parameter integer C_S_AXIS_TDATA_WIDTH	= 32
)
(
    // Users to add ports here
    output wire GET_VOLTAGE,
    // User ports ends
    // Do not modify the ports beyond this line
    // AXI4Stream sink: Clock
    input wire  S_AXIS_ACLK,
    // AXI4Stream sink: Reset
    input wire  S_AXIS_ARESETN,
    // Ready to accept data in
    output wire  S_AXIS_TREADY,
    // Data in
    input wire [C_S_AXIS_TDATA_WIDTH-1 : 0] S_AXIS_TDATA,
    // Byte qualifier
    input wire [(C_S_AXIS_TDATA_WIDTH/8)-1 : 0] S_AXIS_TSTRB,
    // Indicates boundary of last packet
    input wire  S_AXIS_TLAST,
    // Data is in valid
    input wire  S_AXIS_TVALID
);
	
reg [31:0] input_command = 32'h00000000;
reg get_voltage_reg = 1'b0;
wire m_axis_tvalid_input;
reg m_axis_tready_input = 1'b1;
wire [31:0] m_axis_tdata_input;
wire m_axis_tlast_input;


reg [7:0] state = IDLE;
always @(posedge S_AXIS_ACLK) begin
    if(~S_AXIS_ARESETN)begin
        input_command <= 32'h00000000;        
    end
    else begin
        case(state)
            IDLE:begin
                if(m_axis_tvalid_input)begin
                    input_command <= m_axis_tdata_input;
                    m_axis_tready_input <= 1'b0;
                    state <= PROCESSING;
                end
            end
            PROCESSING:begin
                case(input_command)
                    32'h00000000:begin
                        state <= END;
                    end
                    32'h00000001:begin
                        get_voltage_reg <= 1'b1;
                        state <= END;
                    end
                    32'h00000002:begin
                        get_voltage_reg <= 1'b1;
                        input_command <= 32'h00000000;
                        state <= IDLE;
                    end
                    default:begin
                        input_command <= 32'h00000000;
                    end 
                endcase
            end
            END:begin
                get_voltage_reg <= 1'b0;
                input_command <= 32'h00000000;
                m_axis_tready_input <= 1'b1;
                state <= IDLE;
            end
            default:begin
              state <= IDLE;
            end   
        endcase
    end    
end


axis_fifo data_input (
  .s_axis_aresetn(S_AXIS_ARESETN),          // input wire s_axis_aresetn
  .s_axis_aclk(S_AXIS_ACLK),                // input wire s_axis_aclk
  .s_axis_tvalid(S_AXIS_TVALID),            // input wire s_axis_tvalid
  .s_axis_tready(S_AXIS_TREADY),            // output wire s_axis_tready
  .s_axis_tdata(S_AXIS_TDATA),              // input wire [31 : 0] s_axis_tdata
  .s_axis_tlast(S_AXIS_TLAST),              // input wire s_axis_tlast
  .m_axis_tvalid(m_axis_tvalid_input),            // output wire m_axis_tvalid
  .m_axis_tready(m_axis_tready_input),            // input wire m_axis_tready
  .m_axis_tdata(m_axis_tdata_input),              // output wire [31 : 0] m_axis_tdata
  .m_axis_tlast(m_axis_tlast_input),              // output wire m_axis_tlast
  .axis_data_count(),        // output wire [31 : 0] axis_data_count
  .axis_wr_data_count(),  // output wire [31 : 0] axis_wr_data_count
  .axis_rd_data_count()  // output wire [31 : 0] axis_rd_data_count
);

assign GET_VOLTAGE = get_voltage_reg;

endmodule
