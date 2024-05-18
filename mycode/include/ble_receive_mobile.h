// Starts a BLE scan in passive mode.
void start_scan(void);
// Function to start BLE scan with filter.
void start_scan_filter(void);
// Initializes BLE scanning with a specific address filter.
void scan_init(char *addr);
// Returns manufacturer data as one integer.
extern int get_manufact_data(void);
// Returns rssi value as a integer.
extern int8_t get_rssi_data(void);