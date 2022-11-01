
// Lora SENDing
void payload_encoder();
void payload_sender();
void lora_network_joiner();
void lora_join_status();

// Sensor Data collection
void converting_anlouge_signal_to_digital();
void take_measurement(); // measurement for 30 - 60 sec
void high_sample_rate(); // every 20 minutes take measurement alert mode
void normal_sample_rate(); // every 3 hours
void send_error_message();
// void checking_rate();

// Data Storage
void long_data_storage();
void temp_payload_storage();

// Data Display -> custom pcb
void wakeup();
void lora_connection_status();
void charging_status();
void power_status();
void error_status(); //check ubidots
// RAKboard