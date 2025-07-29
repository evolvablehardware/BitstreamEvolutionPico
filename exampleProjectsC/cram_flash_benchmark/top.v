/* Blink the RGB LED */

module top (
  input CLK,
  output LED_B
);
  reg [31:0] counter=0;

  always@(posedge clock) begin
    counter <= counter +1;
  end

  assign {LED_B} = counter[24];

endmodule
