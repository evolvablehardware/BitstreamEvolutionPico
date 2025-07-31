/* Blink the RGB LED */


module top (
  input CLK,
  output LED_R,
  output LED_G,
  output LED_B,
);

  localparam N = 24;

  reg [N:0] counter;

  always @(posedge CLK) begin
    counter <= counter + 1;
  end

  assign LED_R = 1'bz;
  assign LED_G = 1'bz;
  assign LED_B = 1'b0;

endmodule
