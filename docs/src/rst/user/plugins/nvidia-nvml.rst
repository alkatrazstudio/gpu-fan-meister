nvidia-nvml
===========

This plugin, which comes with |project|, allows you to control NVIDIA video cards with official proprietary drivers installed.

The following things are required to operate this plugin:

* NVIDIA Kepler or newer architecture graphics cards (GeForce 600 series and later).
  At your own risk, you can try this plugin with Fermi architecture cards (GeForce 400 and 500 series).

* NVIDIA proprietary drivers installed in any way available:

    * Official website:

        * https://www.geforce.com/drivers

        * https://www.nvidia.ru/Download/index.aspx

    * Proprietary GPU Drivers PPA: https://launchpad.net/~graphics-drivers/+archive/ubuntu/ppa

    * Package manager or other system utilities (for example, Driver Manager in Ubuntu).


* The NVIDIA X Server Settings program that is available via the "nvidia-settings" command (i.e. the program must be located in one of the $PATH directories).

* An NVIDIA X Configuration Tool application that can be accessed via the "nvidia-xconfig" command (i.e. the program must be located in one of the $PATH directories).

* Fan control enabled. This is done by setting the second bit of the CoolBits variable to Xorg.conf.
  More information about CoolBits can be found here: `<https://wiki.archlinux.org/index.php/NVIDIA/Tips_and_tricks#Enabling_overclocking>`_.
  The easiest way to set the second bit in CoolBits: ``sudo nvidia-xconfig --cool-bits=4``. After that it is necessary to reboot the computer.


Plugin functions
----------------

The nvidia-nvml plugin contains one function that can be run from the :doc:`script </user/modes/script>` through :ref:`$.dev.customFunction <script-ref-dev-customFunction>`:

* getThrottleReasons() : Object
    Gives reasons for the throttling of the video card.
    The result of the function is an object that contains throttling reason IDs as keys.
    Each field is an integer value. Each field can have one of two values:

        * **Zero** - throttling, if exists, is not caused by this reason.

        * **Cause code** - throttling is caused by this reason.

    Possible string values:

    * **gpuIdle** - the frequency of the GPU is reduced because no tasks are running on the GPU.

    * **applicationsClocksSetting** - the frequency of the GPU is limited by the general frequency setting for applications.

    * **swPowerCap** - the power-scaling algorithm lowers the GPU frequency.

    * **hwSlowdown** - hardware-initiated slowdown; reasons can be different:

        * the temperature is too high

        * the current is too high

        * PCIe bus or power supply throttling

        * GPU is in a process of switching frequencies or power modes

    * **syncBoost** - the video card is placed in the "Sync boost" group.
      All video cards in this group have the same maximum frequency.
      The **syncBoost** flag indicates that other video cards in this group slow down this video card.

    * **swThermalSlowdown** - driver-initiated frequency reduction due to high GPU or memory temperature.

    * **hwThermalSlowdown** - hardware-initiated frequency reduction due to high temperature.

    * **hwPowerBrakeSlowdown** - PCIe bus or power supply throttling.

    * **unknown** - the throttling is detected, but the reason couldn't be determined.

    * **code** - code of reasons of throttling, which is a logical OR all the IDs of individual reasons of throttling mentioned above. If this field is zero, there is no throttling.

    *Example:*

    .. code-block:: javascript

        const reasons = $.dev.customFunction('getThrottleReasons')
        if(reasons.swThermalSlowdown || reasons.hwThermalSlowdown)
            console.log(`Throttling because of the high temperature! Code: 0x${reasons.code.toString(16)}`)
