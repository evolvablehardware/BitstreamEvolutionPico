/* Blink the RGB LED */

module top (
    input CLK,
    output LED_R
    output LED_G
    output LED_B
);
    localparam N = 26;

    reg [N-1:0] counter;

    always @(posedge CLK) begin
        counter <= counter + 1;
    end

    assign LED_R = counter[N - 1];
    assign LED_G = counter[N - 2];
    assign LED_B = counter[N - 3];
endmodule
