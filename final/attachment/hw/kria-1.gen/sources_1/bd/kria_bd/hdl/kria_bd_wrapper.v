//Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
//--------------------------------------------------------------------------------
//Tool Version: Vivado v.2022.2 (lin64) Build 3671981 Fri Oct 14 04:59:54 MDT 2022
//Date        : Thu Apr 13 16:39:03 2023
//Host        : linux-20 running 64-bit Ubuntu 20.04.1 LTS
//Command     : generate_target kria_bd_wrapper.bd
//Design      : kria_bd_wrapper
//Purpose     : IP block netlist
//--------------------------------------------------------------------------------
`timescale 1 ps / 1 ps

module kria_bd_wrapper
   (fan_en_b,
    miso_0,
    mosi_0,
    pmod1_tri_io,
    pmod2_tri_io,
    spi_0_clk,
    ss_0);
  output [0:0]fan_en_b;
  input miso_0;
  output mosi_0;
  inout [7:0]pmod1_tri_io;
  inout [7:0]pmod2_tri_io;
  output spi_0_clk;
  output [3:0]ss_0;

  wire [0:0]fan_en_b;
  wire miso_0;
  wire mosi_0;
  wire [0:0]pmod1_tri_i_0;
  wire [1:1]pmod1_tri_i_1;
  wire [2:2]pmod1_tri_i_2;
  wire [3:3]pmod1_tri_i_3;
  wire [4:4]pmod1_tri_i_4;
  wire [5:5]pmod1_tri_i_5;
  wire [6:6]pmod1_tri_i_6;
  wire [7:7]pmod1_tri_i_7;
  wire [0:0]pmod1_tri_io_0;
  wire [1:1]pmod1_tri_io_1;
  wire [2:2]pmod1_tri_io_2;
  wire [3:3]pmod1_tri_io_3;
  wire [4:4]pmod1_tri_io_4;
  wire [5:5]pmod1_tri_io_5;
  wire [6:6]pmod1_tri_io_6;
  wire [7:7]pmod1_tri_io_7;
  wire [0:0]pmod1_tri_o_0;
  wire [1:1]pmod1_tri_o_1;
  wire [2:2]pmod1_tri_o_2;
  wire [3:3]pmod1_tri_o_3;
  wire [4:4]pmod1_tri_o_4;
  wire [5:5]pmod1_tri_o_5;
  wire [6:6]pmod1_tri_o_6;
  wire [7:7]pmod1_tri_o_7;
  wire [0:0]pmod1_tri_t_0;
  wire [1:1]pmod1_tri_t_1;
  wire [2:2]pmod1_tri_t_2;
  wire [3:3]pmod1_tri_t_3;
  wire [4:4]pmod1_tri_t_4;
  wire [5:5]pmod1_tri_t_5;
  wire [6:6]pmod1_tri_t_6;
  wire [7:7]pmod1_tri_t_7;
  wire [0:0]pmod2_tri_i_0;
  wire [1:1]pmod2_tri_i_1;
  wire [2:2]pmod2_tri_i_2;
  wire [3:3]pmod2_tri_i_3;
  wire [4:4]pmod2_tri_i_4;
  wire [5:5]pmod2_tri_i_5;
  wire [6:6]pmod2_tri_i_6;
  wire [7:7]pmod2_tri_i_7;
  wire [0:0]pmod2_tri_io_0;
  wire [1:1]pmod2_tri_io_1;
  wire [2:2]pmod2_tri_io_2;
  wire [3:3]pmod2_tri_io_3;
  wire [4:4]pmod2_tri_io_4;
  wire [5:5]pmod2_tri_io_5;
  wire [6:6]pmod2_tri_io_6;
  wire [7:7]pmod2_tri_io_7;
  wire [0:0]pmod2_tri_o_0;
  wire [1:1]pmod2_tri_o_1;
  wire [2:2]pmod2_tri_o_2;
  wire [3:3]pmod2_tri_o_3;
  wire [4:4]pmod2_tri_o_4;
  wire [5:5]pmod2_tri_o_5;
  wire [6:6]pmod2_tri_o_6;
  wire [7:7]pmod2_tri_o_7;
  wire [0:0]pmod2_tri_t_0;
  wire [1:1]pmod2_tri_t_1;
  wire [2:2]pmod2_tri_t_2;
  wire [3:3]pmod2_tri_t_3;
  wire [4:4]pmod2_tri_t_4;
  wire [5:5]pmod2_tri_t_5;
  wire [6:6]pmod2_tri_t_6;
  wire [7:7]pmod2_tri_t_7;
  wire spi_0_clk;
  wire [3:0]ss_0;

  kria_bd kria_bd_i
       (.fan_en_b(fan_en_b),
        .miso_0(miso_0),
        .mosi_0(mosi_0),
        .pmod1_tri_i({pmod1_tri_i_7,pmod1_tri_i_6,pmod1_tri_i_5,pmod1_tri_i_4,pmod1_tri_i_3,pmod1_tri_i_2,pmod1_tri_i_1,pmod1_tri_i_0}),
        .pmod1_tri_o({pmod1_tri_o_7,pmod1_tri_o_6,pmod1_tri_o_5,pmod1_tri_o_4,pmod1_tri_o_3,pmod1_tri_o_2,pmod1_tri_o_1,pmod1_tri_o_0}),
        .pmod1_tri_t({pmod1_tri_t_7,pmod1_tri_t_6,pmod1_tri_t_5,pmod1_tri_t_4,pmod1_tri_t_3,pmod1_tri_t_2,pmod1_tri_t_1,pmod1_tri_t_0}),
        .pmod2_tri_i({pmod2_tri_i_7,pmod2_tri_i_6,pmod2_tri_i_5,pmod2_tri_i_4,pmod2_tri_i_3,pmod2_tri_i_2,pmod2_tri_i_1,pmod2_tri_i_0}),
        .pmod2_tri_o({pmod2_tri_o_7,pmod2_tri_o_6,pmod2_tri_o_5,pmod2_tri_o_4,pmod2_tri_o_3,pmod2_tri_o_2,pmod2_tri_o_1,pmod2_tri_o_0}),
        .pmod2_tri_t({pmod2_tri_t_7,pmod2_tri_t_6,pmod2_tri_t_5,pmod2_tri_t_4,pmod2_tri_t_3,pmod2_tri_t_2,pmod2_tri_t_1,pmod2_tri_t_0}),
        .spi_0_clk(spi_0_clk),
        .ss_0(ss_0));
  IOBUF pmod1_tri_iobuf_0
       (.I(pmod1_tri_o_0),
        .IO(pmod1_tri_io[0]),
        .O(pmod1_tri_i_0),
        .T(pmod1_tri_t_0));
  IOBUF pmod1_tri_iobuf_1
       (.I(pmod1_tri_o_1),
        .IO(pmod1_tri_io[1]),
        .O(pmod1_tri_i_1),
        .T(pmod1_tri_t_1));
  IOBUF pmod1_tri_iobuf_2
       (.I(pmod1_tri_o_2),
        .IO(pmod1_tri_io[2]),
        .O(pmod1_tri_i_2),
        .T(pmod1_tri_t_2));
  IOBUF pmod1_tri_iobuf_3
       (.I(pmod1_tri_o_3),
        .IO(pmod1_tri_io[3]),
        .O(pmod1_tri_i_3),
        .T(pmod1_tri_t_3));
  IOBUF pmod1_tri_iobuf_4
       (.I(pmod1_tri_o_4),
        .IO(pmod1_tri_io[4]),
        .O(pmod1_tri_i_4),
        .T(pmod1_tri_t_4));
  IOBUF pmod1_tri_iobuf_5
       (.I(pmod1_tri_o_5),
        .IO(pmod1_tri_io[5]),
        .O(pmod1_tri_i_5),
        .T(pmod1_tri_t_5));
  IOBUF pmod1_tri_iobuf_6
       (.I(pmod1_tri_o_6),
        .IO(pmod1_tri_io[6]),
        .O(pmod1_tri_i_6),
        .T(pmod1_tri_t_6));
  IOBUF pmod1_tri_iobuf_7
       (.I(pmod1_tri_o_7),
        .IO(pmod1_tri_io[7]),
        .O(pmod1_tri_i_7),
        .T(pmod1_tri_t_7));
  IOBUF pmod2_tri_iobuf_0
       (.I(pmod2_tri_o_0),
        .IO(pmod2_tri_io[0]),
        .O(pmod2_tri_i_0),
        .T(pmod2_tri_t_0));
  IOBUF pmod2_tri_iobuf_1
       (.I(pmod2_tri_o_1),
        .IO(pmod2_tri_io[1]),
        .O(pmod2_tri_i_1),
        .T(pmod2_tri_t_1));
  IOBUF pmod2_tri_iobuf_2
       (.I(pmod2_tri_o_2),
        .IO(pmod2_tri_io[2]),
        .O(pmod2_tri_i_2),
        .T(pmod2_tri_t_2));
  IOBUF pmod2_tri_iobuf_3
       (.I(pmod2_tri_o_3),
        .IO(pmod2_tri_io[3]),
        .O(pmod2_tri_i_3),
        .T(pmod2_tri_t_3));
  IOBUF pmod2_tri_iobuf_4
       (.I(pmod2_tri_o_4),
        .IO(pmod2_tri_io[4]),
        .O(pmod2_tri_i_4),
        .T(pmod2_tri_t_4));
  IOBUF pmod2_tri_iobuf_5
       (.I(pmod2_tri_o_5),
        .IO(pmod2_tri_io[5]),
        .O(pmod2_tri_i_5),
        .T(pmod2_tri_t_5));
  IOBUF pmod2_tri_iobuf_6
       (.I(pmod2_tri_o_6),
        .IO(pmod2_tri_io[6]),
        .O(pmod2_tri_i_6),
        .T(pmod2_tri_t_6));
  IOBUF pmod2_tri_iobuf_7
       (.I(pmod2_tri_o_7),
        .IO(pmod2_tri_io[7]),
        .O(pmod2_tri_i_7),
        .T(pmod2_tri_t_7));
endmodule
