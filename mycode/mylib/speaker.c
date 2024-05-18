#include <zephyr/drivers/pwm.h>
#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/types.h>
#include <stddef.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <stdint.h>
#include <zephyr/sys/slist.h>
#include <zephyr/audio/codec.h>

// Define the PWM device name
#define PWM_DEV_NAME DT_LABEL(DT_ALIAS(pwm_0))
// #define SPEAKER_NODE DT_NODELABEL(spk-pwr-ctrl)
#define SPEAKER_NODE DT_NODELABEL(spk_pwr)
// #define SPEAKER_NODE DT_NODELABEL(spk_pwr)
// const struct DT_LABEL(DT_ALIAS(spk0))
// static const struct pwm_dt_spec pwm_spk = PWM_DT_SPEC_GET(DT_ALIAS(spk0));
// static const struct pwm_dt_spec pwm = PWM_DT_SPEC_GET(DT_NODELABEL(spk_pwr));
// static const struct gpio_dt_spec sw0_gpio = GPIO_DT_SPEC_GET(DT_ALIAS(pwm_0), gpios);

const struct device *pwm_dev;
extern int speaker_init(void)
{

    // Get the struct device for the speaker node
    // pwm_dev = device_get_binding(SPEAKER_NODE);

    pwm_dev = DEVICE_DT_GET(SPEAKER_NODE);
    if (!pwm_dev)
    {
        printk("Failed to get device for speaker\n");
        return -ENODEV;
    }
}

extern int speaker_activate(void)
{
    int ret = 0;

    ret = pwm_set(pwm_dev, 0, 3000, 1500, PWM_POLARITY_NORMAL); // Set PWM parameters for channel 0
    // ret = gpio_pin_configure(gpio_dev, 29, GPIO_OUTPUT_ACTIVE | GPIO_ACTIVE_HIGH);
    // ret = gpio_pin_set_dt(gpio_dev, 1);
    // pwm_set_dt(&pwm, 2000, 1000);
    //  ret = pwm_set(&pwm_spk, 1000, 10000);
    if (ret < 0)
    {
        printk("Error setting PWM parameters%d\n", ret);
        return;
    }
}

extern int speaker_deactivate(void)
{
    int ret = 0;

    ret = pwm_set(pwm_dev, 0, 3000, 0, PWM_POLARITY_NORMAL); // Set PWM parameters for channel 0INVERTED
    // ret = gpio_pin_set_dt(gpio_dev, 1);
    //  pwm_set_pulse_dt(&pwm, 0);
    //   ret = pwm_set(&pwm_spk, 1000, 10000);
    if (ret < 0)
    {
        printk("Error setting PWM parameters%d\n", ret);
        return;
    }
}