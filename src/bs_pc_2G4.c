/*
 * Copyright 2018 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "bs_pc_2G4_types.h"
#include "bs_pc_2G4.h"

/**
 * Family of functions with callbacks:
 * (just a call thru to the stateless version library)
 */
//State for the family of functions with callbacks
static p2G4_dev_state_s_t C2G4_dev_st = {0};

int p2G4_dev_initcom_c(uint d, const char* s, const char* p, dev_abort_reeval_f abort_f) {
  return p2G4_dev_initcom_s_c(&C2G4_dev_st, d, s, p, abort_f);
}

void p2G4_dev_terminate_c(){
  p2G4_dev_terminate_s_c(&C2G4_dev_st);
}

void p2G4_dev_disconnect_c(){
  p2G4_dev_disconnect_s_c(&C2G4_dev_st);
}

int p2G4_dev_req_tx_c_b(p2G4_tx_t *tx_s, uint8_t *packet, p2G4_tx_done_t *tx_done_s) {
  return p2G4_dev_req_tx_s_c_b(&C2G4_dev_st, tx_s, packet, tx_done_s);
}

int p2G4_dev_req_rx_c_b(p2G4_rx_t *rx_s, p2G4_rx_done_t *rx_done_s, uint8_t **buf, size_t size,
                        device_eval_rx_f eval_f){
  return p2G4_dev_req_rx_s_c_b(&C2G4_dev_st, rx_s, rx_done_s, buf, size, eval_f);
}

int p2G4_dev_req_RSSI_c_b(p2G4_rssi_t *RSSI_s, p2G4_rssi_done_t *RSSI_done_s) {
  return p2G4_dev_req_RSSI_s_c_b(&C2G4_dev_st, RSSI_s, RSSI_done_s);
}

int p2G4_dev_req_wait_c_b(pb_wait_t *wait_s){
  return p2G4_dev_req_wait_s_c_b(&C2G4_dev_st, wait_s);
}


/*
 * Set of functions without callbacks:
 * (call thru to stateless version library)
 */
//State for the family of functons without callbacks
static p2G4_dev_state_nc_t C2G4_dev_st_nc = {{0}};

int p2G4_dev_initcom_nc(uint d, const char* s, const char* p) {
  C2G4_dev_st_nc.ongoing = Nothing_2G4;
  return p2G4_dev_initCom_s_nc(&C2G4_dev_st_nc, d, s, p);
}

void p2G4_dev_terminate_nc(){
  p2G4_dev_terminate_s_nc(&C2G4_dev_st_nc);
}
void p2G4_dev_disconnect_nc(){
  p2G4_dev_disconnect_s_nc(&C2G4_dev_st_nc);
}

int p2G4_dev_req_tx_nc_b(p2G4_tx_t *tx_s, uint8_t *packet, p2G4_tx_done_t *tx_done_s) {
  return p2G4_dev_req_tx_s_nc_b(&C2G4_dev_st_nc, tx_s, packet, tx_done_s);
}

int p2G4_dev_provide_new_tx_abort_nc_b(p2G4_abort_t * abort){
  return p2G4_dev_provide_new_tx_abort_s_nc_b(&C2G4_dev_st_nc, abort);
}

int p2G4_dev_req_rx_nc_b(p2G4_rx_t *rx_s, p2G4_rx_done_t *rx_done_s, uint8_t **buf, size_t size){
  return p2G4_dev_req_rx_s_nc_b(&C2G4_dev_st_nc, rx_s, rx_done_s, buf, size);
}

int p2G4_dev_rx_cont_after_addr_nc_b(bool accept_rx){
  return p2G4_dev_rx_cont_after_addr_s_nc_b(&C2G4_dev_st_nc, accept_rx);
}

int p2G4_dev_provide_new_rx_abort_nc_b(p2G4_abort_t * abort){
  return p2G4_dev_provide_new_rx_abort_s_nc_b(&C2G4_dev_st_nc, abort);
}

int p2G4_dev_req_RSSI_nc_b(p2G4_rssi_t *RSSI_s, p2G4_rssi_done_t *RSSI_done_s) {
  return p2G4_dev_req_RSSI_s_nc_b(&C2G4_dev_st_nc, RSSI_s, RSSI_done_s);
}

int p2G4_dev_req_wait_nc_b(pb_wait_t *wait_s){
  return p2G4_dev_req_wait_s_nc_b(&C2G4_dev_st_nc, wait_s);
}


/*
 * IEEE 802.15.4 API implementation
 */

/* For 2 bytes long header and BLE Phy, 1MBPS */
#define HEADER_DURATION_US (2 * 8)
/* 1 Byte preamble, 4 bytes address */
#define PREAMBLE_AND_ADDR_DURATION_US ((1 + 4) * 8)

int p802154_dev_initcom(uint device_id, const char *simulation_name, const char *phy_name) {
  return p2G4_dev_initcom_nc(device_id, simulation_name, phy_name);
}

void p802154_dev_terminate(void) {
  p2G4_dev_terminate_nc();
}

void p802154_dev_disconnect(void) {
  p2G4_dev_disconnect_nc();
}

int p802154_dev_req_tx(p802154_tx_t *tx_s, uint8_t *buf, p2G4_tx_done_t *tx_done) {

  p2G4_tx_t p2G4_tx;

  p2G4_tx.start_time = tx_s->start_time;
  p2G4_tx.end_time = tx_s->end_time;
  p2G4_tx.abort = tx_s->abort;
  p2G4_tx.phy_address = (uint32_t)tx_s->preamble_sfd;
  p2G4_tx.radio_params = tx_s->radio_params;
  p2G4_tx.power_level = tx_s->power_level;
  p2G4_tx.packet_size = tx_s->psdu_size;

  return p2G4_dev_req_tx_nc_b(&p2G4_tx, buf, tx_done);
}

int p802154_dev_provide_new_tx_abort(p2G4_abort_t *abort) {
  return p2G4_dev_provide_new_tx_abort_nc_b(abort);
}

int p802154_dev_req_rx(p802154_rx_t *rx_s, p2G4_rx_done_t *rx_done, uint8_t **rx_buf, size_t buf_size) {

  p2G4_rx_t p2G4_rx;

  p2G4_rx.start_time = rx_s->start_time;
  p2G4_rx.scan_duration = rx_s->scan_duration;
  p2G4_rx.radio_params = rx_s->radio_params;
  p2G4_rx.antenna_gain = rx_s->antenna_gain;
  p2G4_rx.bps = rx_s->bps;
  p2G4_rx.abort = rx_s->abort;
  p2G4_rx.phy_address = (uint32_t)rx_s->preamble_sfd;
  p2G4_rx.sync_threshold = 0;
  p2G4_rx.header_threshold = 0;
  p2G4_rx.header_duration = HEADER_DURATION_US;
  p2G4_rx.pream_and_addr_duration = PREAMBLE_AND_ADDR_DURATION_US;

  return p2G4_dev_req_rx_nc_b(&p2G4_rx, rx_done, rx_buf, buf_size);
}

int p802154_dev_preamble_verified(bool is_preamble_valid) {
  return p2G4_dev_rx_cont_after_addr_nc_b(is_preamble_valid);
}

int p802154_dev_provide_new_rx_abort(p2G4_abort_t *abort) {
  return p2G4_dev_provide_new_rx_abort_nc_b(abort);
}

int p802154_dev_req_RSSI(p2G4_rssi_t *rssi, p2G4_rssi_done_t *rssi_done) {
  return p2G4_dev_req_RSSI_nc_b(rssi, rssi_done);
}

int p802154_dev_req_wait(pb_wait_t *wait_s) {
  return p2G4_dev_req_wait_nc_b(wait_s);
}
