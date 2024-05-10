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

extern int speaker_activate(void)
{
    const struct device *pwm_dev;
    int ret;

    // // Initialize PWM device
    // pwm_dev = device_get_binding("spk_pwr");
    // if (!pwm_dev)
    // {
    //     printk("Cannot find PWM device\n");
    //     return;
    // }
    int gpio_pin;
    struct device *gpio_dev;

    // Get the struct device for the speaker node
    // pwm_dev = device_get_binding(SPEAKER_NODE);

    pwm_dev = DEVICE_DT_GET(SPEAKER_NODE);
    if (!pwm_dev)
    {
        printk("Failed to get device for speaker\n");
        return -ENODEV;
    }

    // // Get the GPIO controller device
    // gpio_dev = device_get_binding(DT_GPIO_LABEL(SPEAKER_NODE, enable_gpios));
    // if (!gpio_dev)
    // {
    //     printk("Failed to get GPIO controller\n");
    //     return -ENODEV;
    // }

    // // Get the GPIO pin number
    // gpio_pin = DT_GPIO_PIN(SPEAKER_NODE, enable_gpios);

    // // Set up GPIO pin
    // ret = gpio_pin_configure(gpio_dev, gpio_pin, GPIO_DIR_OUT);
    // if (ret)
    // {
    //     printk("Failed to configure GPIO pin\n");
    //     return ret;
    // }

    // // Register GPIO callback
    // gpio_init_callback(&gpio_cb, gpio_callback, BIT(gpio_pin));
    // ret = gpio_add_callback(gpio_dev, &gpio_cb);
    // if (ret)
    // {
    //     printk("Failed to add GPIO callback\n");
    //     return ret;
    // }

    // // Enable GPIO interrupt
    // ret = gpio_pin_enable_callback(gpio_dev, gpio_pin);
    // if (ret)
    // {
    //     printk("Failed to enable GPIO interrupt\n");
    //     return ret;
    // }

    // struct audio_codec_cfg *cfg;
    // cfg->mclk_freq = 1000;

    // audio_codec_configure(pwm_dev, cfg);

    // audio_codec_start_output(pwm_dev);
    // k_sleep(K_SECONDS(1));
    // audio_codec_stop_output(pwm_dev);
    // // Configure PWM parameters
    // struct pwm_config pwm_cfg = {
    //     .period = 20000,        // Set period (in microseconds), corresponds to 50Hz
    //     .pulse_width = 10000,   // Set pulse width (in microseconds), corresponds to 50% duty cycle
    //     .flags = PWM_DS_INVERT, // Set flags for PWM operation
    // };

    // Set the frequency
    ret = pwm_set(pwm_dev, 0, 2000, 1500, PWM_POLARITY_INVERTED); // Set PWM parameters for channel 0
    // ret = pwm_set(&pwm_spk, 1000, 10000);
    if (ret < 0)
    {
        printk("Error setting PWM parameters%d\n", ret);
        return;
    }

    // // Start PWM
    // ret = pwm_pin_start(pwm_dev, 0); // Start PWM output on channel 0
    // if (ret < 0)
    // {
    //     printk("Error starting PWM\n");
    //     return;
    // }

    // // Delay or perform other operations as needed
    // k_sleep(K_SECONDS(5)); // For example, sleep for 5 seconds

    // // Stop PWM
    // ret = pwm_pin_stop(pwm_dev, 0); // Stop PWM output on channel 0
    // if (ret < 0)
    // {
    //     printk("Error stopping PWM\n");
    //     return;
    // }
}