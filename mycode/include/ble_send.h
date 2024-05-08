// Main task for BLE advertising of iBeacon data.
// extern int tsk_ibeacon(void);
// Task for canning nearby ibeacons
extern int tsk_scan(void);

// Place of data in ibeacon packet
#define DATA_OFFSET 20