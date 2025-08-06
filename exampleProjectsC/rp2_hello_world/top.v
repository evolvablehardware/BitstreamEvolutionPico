/* Blink the RGB LED */

module top (
  input clock,
  input ICE_26, // rp pico pin 4
  input ICE_23, // rp pico pin 5
  input ICE_20_G3, // rp pico pin 6
  output LED_R,
  output LED_G,
  output LED_B,
  output ICE_18
);
  reg [31:0] counter=0;

  always@(posedge clock) begin
    counter <= counter +1;
  end

  assign {LED_R,LED_G,LED_B} = {!ICE_26, !ICE_23, !ICE_20_G3};
  assign ICE_18 = ICE_26 && ICE_20_G3;

endmodule