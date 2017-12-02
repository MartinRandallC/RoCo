#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Chat Example
# Author: Extranjero
# Generated: Fri Dec  1 18:30:21 2017
##################################################

from gnuradio import analog
from gnuradio import blocks
from gnuradio import digital
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio import uhd
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from optparse import OptionParser
import gwn
import gwnutils
import time


class chat_example(gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self, "Chat Example")

        ##################################################
        # Variables
        ##################################################
        self.samp_rate = samp_rate = .200e6
        self.debug = debug = True
        self.center_freq = center_freq = 400e6
        self.access_code = access_code = gwnutils.default_access_code

        ##################################################
        # Blocks
        ##################################################
        self.analog_probe_avg_mag_sqrd_x_0 = analog.probe_avg_mag_sqrd_c(-140, 1)
        self.uhd_usrp_source_0 = uhd.usrp_source(
        	",".join(("", "")),
        	uhd.stream_args(
        		cpu_format="fc32",
        		channels=range(1),
        	),
        )
        self.uhd_usrp_source_0.set_samp_rate(samp_rate)
        self.uhd_usrp_source_0.set_center_freq(center_freq, 0)
        self.uhd_usrp_source_0.set_gain(40, 0)
        self.uhd_usrp_source_0.set_antenna("RX2", 0)
        self.uhd_usrp_sink_0 = uhd.usrp_sink(
        	",".join(("", "")),
        	uhd.stream_args(
        		cpu_format="fc32",
        		channels=range(1),
        	),
        )
        self.uhd_usrp_sink_0.set_samp_rate(samp_rate)
        self.uhd_usrp_sink_0.set_center_freq(center_freq, 0)
        self.uhd_usrp_sink_0.set_gain(70, 0)
        self.uhd_usrp_sink_0.set_antenna("TX/RX", 0)
        self.gwn_pdu_to_ev_0 = gwn.pdu_to_ev('payload')
        self.gwn_l1_framer_0 = gwn.l1_framer('event', False)
        self.gwn_l1_deframer_0 = gwn.l1_deframer('event', False)
        self.gwn_hier_tx_psk_0 = gwn.hier_tx_psk(0.35, 3,  2 , ([-1-1j, 1-1j, 1+1j, -1+1j]),  7 ,  0.4 )
        self.gwn_hier_rx_psk_0 = gwn.hier_rx_psk( 2*3.14/100 ,  3.14/1600 ,  2*3.14/100 ,  32 ,  7 , digital.constellation_calcdist([-1-1j, 1-1j, 1+1j, -1+1j], [], 4, 1).base(), 3, 0.35,  2 , 6e-1, 6e-1, 2.0, 15.0, 0.1, 0)
        self.gwn_event_guider_0 = gwn.event_guider('DataData', 'CtrlACK', 'Marten', False, debug)
        self.gwn_event_constructor_0 = gwn.event_constructor('Marten', False, 1, 'Mariana', debug)
        self.gwn_ev_to_pdu_0 = gwn.ev_to_pdu('payload')
        self.gwn_ack_rx_0 = gwn.ack_rx('CtrlACK', debug)
        self.gwn_CSMA_FSM_0 = gwn.CSMA_FSM('CtrlACK', 1000, 'EventTimer', 0.3, 'TimerTOH', 0.1, 1000, 0.1, False, debug)
        
        get_level = self.analog_probe_avg_mag_sqrd_x_0.level
        print(str(get_level))
        
        try:
            self.gwn_CSMA_FSM_0.set_get_level(get_level)
        except AttributeError:
            print "Probe Medium XML, AttributeError in setting get_level function"
        except:
            print "Probe Medium XML, another error in setting get_level function"
          
        self.digital_correlate_access_code_xx_ts_0 = digital.correlate_access_code_bb_ts(access_code,
          1, "correlate")
        self.blocks_tagged_stream_to_pdu_0 = blocks.tagged_stream_to_pdu(blocks.byte_t, "correlate")
        self.blocks_socket_pdu_1 = blocks.socket_pdu("TCP_SERVER", "127.0.0.1", "50007", 10000, False)
        self.blocks_pdu_to_tagged_stream_0 = blocks.pdu_to_tagged_stream(blocks.byte_t, "packet_len")

        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.blocks_socket_pdu_1, 'pdus'), (self.gwn_pdu_to_ev_0, 'pdu'))    
        self.msg_connect((self.blocks_tagged_stream_to_pdu_0, 'pdus'), (self.gwn_l1_deframer_0, 'pdu'))    
        self.msg_connect((self.gwn_CSMA_FSM_0, 'out1'), (self.gwn_ev_to_pdu_0, 'in0'))    
        self.msg_connect((self.gwn_CSMA_FSM_0, 'out0'), (self.gwn_l1_framer_0, 'in0'))    
        self.msg_connect((self.gwn_ack_rx_0, 'out0'), (self.gwn_ev_to_pdu_0, 'in0'))    
        self.msg_connect((self.gwn_ack_rx_0, 'out1'), (self.gwn_l1_framer_0, 'in0'))    
        self.msg_connect((self.gwn_ev_to_pdu_0, 'pdu'), (self.blocks_socket_pdu_1, 'pdus'))    
        self.msg_connect((self.gwn_event_constructor_0, 'out0'), (self.gwn_CSMA_FSM_0, 'in0'))    
        self.msg_connect((self.gwn_event_guider_0, 'out1'), (self.gwn_CSMA_FSM_0, 'in0'))    
        self.msg_connect((self.gwn_event_guider_0, 'out0'), (self.gwn_ack_rx_0, 'in0'))    
        self.msg_connect((self.gwn_event_guider_0, 'out2'), (self.gwn_l1_framer_0, 'in0'))    
        self.msg_connect((self.gwn_l1_deframer_0, 'out0'), (self.gwn_event_guider_0, 'in0'))    
        self.msg_connect((self.gwn_l1_framer_0, 'pdu'), (self.blocks_pdu_to_tagged_stream_0, 'pdus'))    
        self.msg_connect((self.gwn_pdu_to_ev_0, 'out0'), (self.gwn_event_constructor_0, 'in0'))    
        self.connect((self.blocks_pdu_to_tagged_stream_0, 0), (self.gwn_hier_tx_psk_0, 0))    
        self.connect((self.digital_correlate_access_code_xx_ts_0, 0), (self.blocks_tagged_stream_to_pdu_0, 0))    
        self.connect((self.gwn_hier_rx_psk_0, 0), (self.digital_correlate_access_code_xx_ts_0, 0))    
        self.connect((self.gwn_hier_tx_psk_0, 0), (self.uhd_usrp_sink_0, 0))    
        self.connect((self.uhd_usrp_source_0, 0), (self.analog_probe_avg_mag_sqrd_x_0, 0))    
        self.connect((self.uhd_usrp_source_0, 0), (self.gwn_hier_rx_psk_0, 0))    

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.uhd_usrp_sink_0.set_samp_rate(self.samp_rate)
        self.uhd_usrp_source_0.set_samp_rate(self.samp_rate)

    def get_debug(self):
        return self.debug

    def set_debug(self, debug):
        self.debug = debug

    def get_center_freq(self):
        return self.center_freq

    def set_center_freq(self, center_freq):
        self.center_freq = center_freq
        self.uhd_usrp_sink_0.set_center_freq(self.center_freq, 0)
        self.uhd_usrp_source_0.set_center_freq(self.center_freq, 0)

    def get_access_code(self):
        return self.access_code

    def set_access_code(self, access_code):
        self.access_code = access_code


def main(top_block_cls=chat_example, options=None):

    tb = top_block_cls()
    tb.start()
    tb.wait()


if __name__ == '__main__':
    main()
