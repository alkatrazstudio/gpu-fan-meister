Script objects reference
========================

The script has access to almost all of the settings of the application, the current GPU, and individual modes that are available from the GUI. Also, there is access to some system functions. All these functions and parameters are accessible through global object ``$``. Below, in the documentation, if not specified otherwise, it is considered that object fields are read-only.

In addition, there is access to most standard ECMAScript functions and objects. The list of supported ECMAScript objects can be found here: `<https://doc.qt.io/qt-5.12/ecmascript.html>`_.


.. contents:: The following objects can be accessed via script:
    :local:
    :backlinks: entry


Constants
---------

.. _script-ref-dev-fanSpeedGpu:

* $.dev.FAN_SPEED_GPU : Number
    The script should return this constant from the main function if you need to pass the fan control to the video card during this step of the script.


.. _script-ref-dev-fanSpeedNoChange:

* $.dev.FAN_SPEED_NO_CHANGE : Number
    The script should return this constant from the main function if at this step of the script the program should not influence the speed of the fans in any way.
    Thus, speed of fans can change nevertheless if the fans are currently controlled by video card (for example, on the previous step the script main function has returned :ref:`$.dev.FAN_SPEED_GPU <script-ref-dev-fanSpeedGpu>`).


* **$.dev.MODE_CURVE : String**
* **$.dev.MODE_FAN_TOGGLE : String**
* **$.dev.MODE_SCRIPT : String**
* **$.dev.MODE_GPU : String**
    The constants describing application :doc:`modes <../../modes>`. For more details, see :ref:`description of $.dev.mode <script-ref-dev-mode>`.


* **$.dev.SCRIPT_STATUS_STOPPED: String**
* **$.sys.SCRIPT_STATUS_PAUSED: String**
* **$.sys.SCRIPT_STATUS_RUNNING: String**

    Constants that indicate the status of the script. For more information, see :ref:`description of $.dev.scriptStatus <script-ref-dev-scriptStatus>`.


* **$.sys.FAILED_AT_START: String**
* **$.sys.FAILED_AT_WRITE: String**
* **$.sys.FAILED_AT_FINISH: String**
    Constants indicating the stage at which the execution of an external program launched from the script was interrupted. For more details, see :ref:`the corresponding section <script-ref-sys-exec-failedAt>` of the **$.sys.exec** function description.


Current GPU details
-------------------

* $.dev.id : String
    Unique GPU identifier.

    *Example:* :code:`"GPU-c3b01b58-4dbf-1e8f-8f72-a8360f030d4c"`


* $.dev.realName : String
    The name of the GPU specified by its manufacturer.

    *Example:* :code:`"GeForce GTX 1060 6GB"`


* $.dev.name : String
    The name of the GPU that appears in the graphical interface of the application.
    If the user has specified the custom name (**$.dev.customName** is not an empty string), then this name will be **$.dev.name**, otherwise it will be the value of **$.dev.realName**.

    *Example:* :code:`"My Fast GPU :)"`


* $.dev.temp : Number
    Current GPU temperature in degrees Celsius.


* $.dev.fanSpeed : Number
    The current fan speed in the range from 0 to 100.
    It may differ from the speed returned by the :ref:`main function <mode-script-mainFunction>`, depending on the speed actually supported by the video card.


* $.dev.fanManualMode : Boolean
    If **true**, the fans are controlled by the application.
    If **false** - the fans are controlled by the GPU.


.. _script-ref-dev-targetFanSpeed:

* $.dev.targetFanSpeed : Number
    The fan speed the current mode wants to set.
    Unlike $.dev.fanSpeed, it can take :ref:`$.dev.FAN_SPEED_GPU <script-ref-dev-fanSpeedGpu>` and :ref:`$.dev.FAN_SPEED_NO_CHANGE <script-ref-dev-fanSpeedNoChange>` values.


.. _script-ref-dev-mode:

* $.dev.mode : String |rw|
    Current :doc:`mode <../../modes>` for the current video card.

    .. list-table:: Possible values
        :header-rows: 1

        * - Value
          - Corresponding constant
          - Mode
        * - ``"curve"``
          - ``$.dev.MODE_CURVE``
          - :doc:`Curve <../curve>`
        * - ``"fanToggle"``
          - ``$.dev.MODE_FAN_TOGGLE``
          - :doc:`Fan toggle <../fanToggle>`
        * - ``"script"``
          - ``$.dev.MODE_SCRIPT``
          - :doc:`Script <../script>`
        * - ``"gpu"``
          - ``$.dev.MODE_GPU``
          - :doc:`GPU controlled <../gpu>`

    *Example:*

    .. code-block:: javascript

        if($.dev.mode == $.dev.MODE_CURVE)
            console.log('We are in the "Curve" mode now')


.. _script-ref-dev-gpuRate:

* $.dev.gpuRate : Number
    Percentage of time in the last sampling period (usually 1 second) during which at least one GPU core has been used.
    If :ref:`$.dev.plugin.canGetGpuRate <script-ref-dev-plugin-canGetGpuRate>` = **false**, this value is always zero.

    *Example:*

    .. code-block:: javascript

        console.log(`Processor ${$.dev.name} load is ${$.dev.gpuRate}%`)


.. _script-ref-dev-memoryRate:

* $.dev.memoryRate : Number
    Percentage of time in the last sampling period (typically 1 second) during which GPU memory was read or written.
    If :ref:`$.dev.plugin.canGetMemoryRate <script-ref-dev-plugin-canGetMemoryRate>` = **false**, this value is always zero.

    *Example:*

    .. code-block:: javascript

        console.log(`Memory ${$.dev.name} load is ${$.dev.memoryRate}%`)


.. _script-ref-dev-totalMemory:

* $.dev.totalMemory : Number
    Total amount of memory in bytes.
    If :ref:`$.dev.plugin.canGetTotalMemory <script-ref-dev-plugin-canGetTotalMemory>` = **false**, this value is always zero.


.. _script-ref-dev-freeMemory:

* $.dev.freeMemory : Number
    Total amount of free memory in bytes.
    If :ref:`$.dev.plugin.canGetFreeMemory <script-ref-dev-plugin-canGetFreeMemory>` = **false**, this value is always zero.


Functions for working with GPU
------------------------------

* $.dev.loadGeneralSettings()
    Download previously saved general video card settings.


* $.dev.saveGeneralSettings()
    Save the general application settings to a file.


* $.dev.loadModeSettings()
    Restore application mode settings.
    These settings are located in the tabs of the main program window.
    Execution of this function is equivalent to pressing the "Cancel" button in this window, but only without closing the window later.


* $.dev.saveModeSettings()
    Save the application mode settings.
    These settings are located in the tabs of the main program window.
    Execution of this function is equal to pressing the "Save" button in this window, but only without closing the window later.


* $.dev.restartScript()
    Launches new script with a source code from **$.dev.scriptText**.
    The current script will be run to the end and then stopped.


* $.dev.stopScript()
    Stops the current script after it has been executed to the end.


* $.dev.showNotification(msg: String, isCritical: Boolean)
    Show system notification with text **msg**.
    If **isCritical** = **true**, the message is set to a higher priority (the system will decide how this affects the shown notification).

    Depending on the operating system and its version, system notifications may support some HTML tags, such as &lt;b> or &;lti>, as well as links &lt;a>.

    *Example:*

    .. code-block:: javascript

        // Displaying a message with higher priority
        $.dev.showNotification('Your video card will melt soon!', true)
        
        // Showing HTML messages
        // (may not work or may not show what is needed on some platforms)
        $.dev.showNotification(
            'Here is <a href="https://example.com/">one <i>good</i> example</a>.')


.. _script-ref-dev-customFunction:

* $.dev.customFunction(name: String, val: Any = undefined) : Any
    Start the **name** function for this device and return the result.
    Any value **val** can be optionally passed to the function.
    The list of available functions can be obtained through the property :ref:`$.dev.plugin.customFunctions <script-ref-dev-plugin-customFunctions>`.

    *Example:*

    .. code-block:: javascript

        // Example of a function call from nvidia-nvml plugin
        const reasons = $.dev.customFunction('getThrottleReasons')
        if(reasons.swThermalSlowdown || reasons.hwThermalSlowdown)
            console.log(`Throttling because of the high temperature! Code: 0x${reasons.code.toString(16)}`)


General settings for the video card
-----------------------------------

* $.dev.customName : String |rw|
    The name of the GPU specified by the user.

    *Example:* :code:`"My Fast GPU :)"`


* **$.dev.colorTempNormal: ColorString** |rw|
* **$.dev.colorTempWarning: ColorString** |rw|
* **$.dev.colorTempDanger: ColorString** |rw|
* **$.dev.tempWarning: Number** |rw|
* **$.dev.tempDanger: Number** |rw|
    If the current temperature of the video card (in degrees Celsius) is less than **$.dev.tempWarning**, the number in the tray icon indicating the temperature will be **$.dev.colorTempNormal**.
    If the temperature is greater than or equal to **$.dev.tempWarning**, but not greater than **$.dev.tempDanger**, the number will have the color **$.dev.colorTempWarning**.
    If the temperature is greater than or equal to **$.dev.tempDanger**, the number will have the color **$.dev.colorTempDanger**.
    For more information about the **ColorString** data type see ":ref:`script-ref-tools`".


* **$.dev.colorFanSpeedNormal: ColorString** |rw|
* **$.dev.colorFanSpeedWarning: ColorString** |rw|
* **$.dev.colorFanSpeedDanger: ColorString** |rw|
* **$.dev.fanSpeedWarning: Number** |rw|
* **$.dev.fanSpeedDanger: Number** |rw|
    If the speed of the video card fans (in the range from 0 to 100) is less than **$.dev.fanSpeedWarning**, the tray icon border will be **$.dev.colorFanSpeedNormal**.
    If the speed is greater than or equal to **$.dev.fanSpeedWarning**, but no more than **$.dev.fanSpeedDanger**, the border will have the color **$.dev.colorFanSpeedWarning**.
    If the speed is greater than or equal to **$.dev.fanSpeedDanger**, the border will have the color **$.dev.colorFanSpeedDanger**.
    For more information about the **ColorString** data type see ":ref:`script-ref-tools`".


* **$.dev.colorBackground: ColorString** |rw|
    The background color of the tray icon.


* $.dev.trayUpdateInterval : Number |rw|
    The minimum tray update interval for this video card. Measured in seconds.


"Curve" mode settings
---------------------

These settings apply to the ":doc:`../curve`" mode.

* $.dev.curvePoints : Point[] |rw|
    Stores an array of curve points. The array is represented by the standard **Array** type.
    Each point is an object with **x** and **y** properties, where **x** is the temperature in degrees Celsius, **y** is the speed of the fans in the range from 0 to 100 inclusive.
    The contents of this array cannot be changed directly. Instead, you need to create a copy using the :ref:`$.tools.clone <script-ref-tools-clone>` function, change this copy, and then assign this copy to the **$.dev.curvePoints** array.
    After modifying **$.dev.curvePoints** all the data in it will be normalized relative to the chart boundaries, so the data assigned to this array may differ from the data that will actually be there afterwards.

    *Example:*

    .. code-block:: javascript

        // increase the fan speed for the penultimate point
        var pts = $.tools.clone($.dev.curvePoints)
        pts[pts.length-2].y +=10
        $.dev.curvePoints = pts
        
        // set a new set of points
        $.dev.curvePoints = [
            {x: 0, y: 0,
            {x: 60, y: 0},
            {x: 100, y: 50}
        ]
        
        // attempt to set an incorrect set of points
        $.dev.curvePoints = [
            {x: 20, y: 30},
            50, bogus: 123},
            {x: 100, y: -10}
        ]
        $.dev.curvePoints // [{x: 0, y: 30}, {x: 50, y: 0}, {x: 100, y: 0}]


* $.dev.curveMaxIncTempDiff: Number |rw|
    The value of ":ref:`Max inc. temp. diff. <curve-maxIncTempDiff>`".


* $.dev.curveIncDelay: Number |rw|
    The value of the ":ref:`Inc. delay <curve-incDelay>`" parameter.


* $.dev.curveMaxDecTempDiff: Number |rw|
    The value of ":ref:`Max inc. temp. diff. <curve-maxDecTempDiff>`" parameter.


* $.dev.curveDecDelay: Number |rw|
    The value of the ":ref:`Dec. delay <curve-decDelay>`" parameter.


* $.dev.curveRefTemp: Number
    The temperature that is used to calculate the required fan speed according to the curve. See ":ref:`curve-hysteresis`", where this temperature is indicated as |T1|.


"Fan toggle" mode settings
--------------------------

These settings refer to the ":doc:`../fanToggle`" mode.

.. _script-ref-dev-fanToggleEnableFansTemp:

* fanToggleEnableFansTemp: Number |rw|
    Fans will be enabled if the temperature of the video card is equal or higher than this value for :ref:`fanToggleEnableFansTempSecs <script-ref-dev-fanToggleEnableFansTempSecs>` seconds.


.. _script-ref-dev-fanToggleEnableFansTempSecs:

* fanToggleEnableFansTempSecs: Number |rw|
    Time in seconds after which the fans will be enabled if the temperature has been at least :ref:`fanToggleEnableFansTemp <script-ref-dev-fanToggleEnableFansTemp>` all this time.


.. _script-ref-dev-fanToggleUseLowFanSpeed:

* fanToggleUseLowFanSpeed : Boolean |rw|
    Enable the rule that disable the fans if their speed does not exceed :ref:`fanToggleLowFanSpeed <script-ref-dev-fanToggleLowFanSpeed>` for :ref:`fanToggleLowFanSpeedSecs <script-ref-dev-fanToggleLowFanSpeedSecs>` seconds.


.. _script-ref-dev-fanToggleLowFanSpeed:

* fanToggleLowFanSpeed: Number |rw|
    Fans will be disabled if their speed was less than or equal to this value during :ref:`fanToggleLowFanSpeedSecs <script-ref-dev-fanToggleLowFanSpeedSecs>` seconds.
    Applies only if :ref:`fanToggleUseLowFanSpeed <script-ref-dev-fanToggleUseLowFanSpeed>` = **true**.


.. _script-ref-dev-fanToggleLowFanSpeedSecs:

* fanToggleLowFanSpeedSecs: Number |rw|
    Time in seconds after which the fans will be disabled if their speed does not exceed :ref:`fanToggleLowFanSpeed <script-ref-dev-fanToggleLowFanSpeed>`.
    Applies only if :ref:`fanToggleUseLowFanSpeed <script-ref-dev-fanToggleUseLowFanSpeed>` = **true**.


.. _script-ref-dev-fanToggleUseLowTemp:

* fanToggleUseLowTemp : Boolean |rw|
    Enable the rule that disable the fans if the temperature of the video card does not exceed :ref:`fanToggleLowTemp <script-ref-dev-fanToggleLowTemp>` for :ref:`fanToggleLowTempSecs <script-ref-dev-fanToggleLowTempSecs>` seconds.


.. _script-ref-dev-fanToggleLowTemp:

* fanToggleLowTemp: Number |rw|
    The fans will be disabled if the temperature of the video card does not exceed this value for :ref:`fanToggleLowTempSecs <script-ref-dev-fanToggleLowTempSecs>` seconds.
    Applies only if :ref:`fanToggleUseLowTemp <script-ref-dev-fanToggleUseLowTemp>` = **true**.


.. _script-ref-dev-fanToggleLowTempSecs:

* fanToggleLowTempSecs: Number |rw|
    Time in seconds after which the fans will be disabled if the temperature of the video card does not exceed :ref:`fanToggleLowTemp <script-ref-dev-fanToggleLowTemp>`.
    Applies only if :ref:`fanToggleUseLowTemp <script-ref-dev-fanToggleUseLowTemp>` = **true**.


"Script" mode settings
----------------------

These settings refer to the ":doc:`../script`" mode.

* $.dev.scriptText: String |rw|
    Script source code that is displayed in the editor. This text may not be the same as the text of the currently running script (see the next parameter).


* $.dev.runningScriptText: String
    The source code of the script that was executed last time or is being executed now.


* $.dev.scriptOverride: Boolean |rw|
    Enables the :ref:`"Override" <mode-script-override>` mode.


.. _script-ref-dev-scriptStatus:

* $.dev.scriptStatus: String
    The state of the script when the current step will be completed.
    It can be one of the following values:

    * - Value
      - Corresponding constant
      - Mode
    * - ``"curve"``
      - ``$.dev.SCRIPT_STATUS_STOPPED``
      - Script is :ref:`stopped <mode-script-status-paused>`.
    * - ``"fanToggle"``
      - ``$.dev.SCRIPT_STATUS_PAUSED``
      - Script is :ref:`paused <mode-script-status-paused>`.
    * - ``"script"``
      - ``$.dev.SCRIPT_STATUS_RUNNING``
      - The script is :ref:`running <mode-script-status-running>`.

    *Example:*

    .. code-block:: javascript

        if($.dev.scriptStatus == $.dev.SCRIPT_STATUS_STOPPED)
            console.log('The script will be stopped after the current step')


* $.dev.scriptError: String
    Current error that occurred in the script. Though this property is accessible through a script, for a script it will always be an empty string because the script cannot be started in the presence of errors. This property is used only in internal scripts of the application.


GPU plugin info
---------------

* $.dev.plugin.category: String
    Plugin category. Denotes the class of the GPU that the plugin works with.
    In |project| more than one plugin from a single category cannot be loaded simultaneously .

    *Example:* ``"nvidia"``


* $.dev.plugin.id: String
    Unique plugin identifier.

    *Example:* ``"nvidia-nvml"``


* $.dev.plugin.name
    Plugin name.

    *Example:* ``"NVIDIA (NVML)"``


* $.dev.plugin.description: String
    Plugin description.

    *Example:* ``"NVIDIA video card plugin using NVML library."``


* $.dev.plugin.copyright: String
    Information about the authors of the plugin.

    *Example:* ``"Alkatraz Studio, 2018"``


* $.dev.plugin.misc: String
    Any other information about the plugin that is not suitable for other fields.

    *Example:* ``"Project site: https://example.com\nEmail: user@example.com"``


* $.dev.plugin.version: String
    Plugin version in the `SemVer <https://semver.org>`_ format.

    *Example:* ``"1.0.5"``


* $.dev.plugin.majorVersion: Number
    Major version of the plugin. For example, for version **1.0.5** the major version will be **1**. Read more about versioning at `SemVer <https://semver.org>`_.


* $.dev.plugin.minorVersion: Number
    Minor version of the plugin. For example, for version **1.0.5**, the minor version will be **0**. Read more about versioning at `SemVer <https://semver.org>`_.


* $.dev.plugin.patchVersion: Number
    Patch version of the plugin. For example, for version **1.0.5** the patch version will be **5**. Read more about versioning at `SemVer <https://semver.org>`_.


* $.dev.plugin.buildDate: Date
    Plugin build date and time.
    If this information is not available, the "Invalid Date" object is returned.

    *Example:*

    .. code-block:: javascript

        if(isNaN($.dev.plugin.buildDate))
            console.log('Plugin build date is unavailable')
        else
            console.log(`Plugin build date: ${$.dev.plugin.buildDate}`)


* $.dev.plugin.deviceCount: Number
    The number of video cards the plugin found in the system.


.. _script-ref-dev-plugin-canGetGpuRate:

* $.dev.plugin.canGetGpuRate: Boolean
    If **true**, the field :ref:`$.dev.gpuRate <script-ref-dev-gpuRate>` contains correct data.


.. _script-ref-dev-plugin-canGetMemoryRate:

* $.dev.plugin.canGetMemoryRate: Boolean
    If **true**, the field :ref:`$.dev.memoryRate <script-ref-dev-memoryRate>` contains correct data.


.. _script-ref-dev-plugin-canGetTotalMemory:

* $.dev.plugin.canGetTotalMemory: Boolean
    If **true**, the field :ref:`$.dev.totalMemory <script-ref-dev-totalMemory>` contains correct data.


.. _script-ref-dev-plugin-canGetFreeMemory:

* $.dev.plugin.canGetFreeMemory: Boolean
    If **true**, the field :ref:`$.dev.freeMemory <script-ref-dev-freeMemory>` contains the correct data.


.. _script-ref-dev-plugin-customFunctions:

* $.dev.plugin.customFunctions: Array<String>
    The list of functions that can be run via :ref:`$.dev.customFunction <script-ref-dev-customFunction>`.
    You can find the description of each function in the documentation of the corresponding plugin.


Application info
----------------

* $.app.id: String
    Application ID.

    *Example:* ``"|project-id|"``


* $.app.title
    Application name.

    *Example:* ``"|project|"``


* $.app.author
    Author of the application.

    *Example:* ``"|project-org|"``


* $.app.version: String
    Application version in the `SemVer <https://semver.org>`_ format.

    *Example:* ``"1.0.5"``


* $.app.majorVersion: Number
    Major version of the application.
    For example, for version **1.0.5**, the major version will be **1**.
    Read more about version numbers here: `SemVer <https://semver.org>`_.


* $.app.minorVersion: Number
    Minor version of the application.
    For example, for version **1.0.5**, the minor version will be **0**.
    Read more about version numbers here: `SemVer <https://semver.org>`_.


* $.app.patchVersion: Number
    Patch version of the application.
    For example, for version **1.0.5** the patch version will be **5**.
    Read more about version numbers here: `SemVer <https://semver.org>`_.


System info
-----------

Through the **$.sys** object, the script can access some system features (e.g. operating system name). Properties of the given object correspond to the methods described in `<https://doc.qt.io/qt-5.12/qsysinfo.html>`_.

* $.sys.buildAbi: String
    Full description of the architecture for which the current application is built.

    *Example:* ``"x86_64-little_endian-lp64"``


* $.sys.buildCpuArchitecture: String
    CPU architecture for which the current application is built.

    *Example:* ``"x86_64"``


* $.sys.currentCpuArchitecture: String
    The CPU architecture on which the application is currently running.

    *Example:* ``"x86_64"``


* $.sys.kernelType: String
    Type of operating system kernel.

    *Example:* ``"linux"``


* $.sys.kernelVersion: String
    Operating system kernel version.

    *Example:* ``"4.15.0-34-lowlatency"``


* $.sys.prettyProductName: String
    Name of the operating system (may not correspond to the actual name).

    *Example:* ``"Ubuntu 18.04.1 LTS"``


* $.sys.productType: String
    Type of operating system distribution.

    *Example:* ``"ubuntu"``


* $.sys.productVersion: String
    Operating system version.

    *Example:* ``"18.04"``


System functions
----------------

* $.sys.exec(parts : String|Object, opts : Object = {}) : Object
    Launch an external program.
    In the **parts** array you can specify an executable file (the first element) as well as arguments for the program to be run (the following elements).
    If you don't need arguments, you can specify the executable file as a string instead of an array in **parts**.

    **opts** - options. List of supported options for **$.sys.exec**:

        * env: Object
            A set of environment variables.
            This set will be added to the environment variables with which |project| was launched.
            If the names of the variables match the existing variables, the variables from **env** will overwrite the variables of |project|.

            *Default:* ``{}``

        * detached: Boolean
            Do not wait until the process is completed, and return control of the script immediately.
            You will not be able to get the exit code or data from stdout and stderr.

            *Default:* ``false``

        * timeout: Number
            Maximum allowable program execution time in seconds.
            If the program takes longer to complete than the specified time, it is forcibly terminated.

            *Default:* ``3``

        * stdin: String
            String data in the UTF-8 encoding, which will be transferred to the launched program into the standard stdin input stream.

            *Default:* ``""``


    **$.sys.exec** returns an object that contains information about the launched process:

        * started: Boolean
            If **true**, the program has been successfully launched.
            This does not mean, however, that the program has been successfully completed.

        * crashed: Boolean
            If **true**, an unexpected failure has occurred during the program operation.
            Absent if **opts.detached** = ``true``.

        .. _script-ref-sys-exec-failedAt:

        * failedAt: String
            State name of the stage at which the error occurred in the program.

            .. list-table:: Possible values
                :header-rows: 1

                * - Value
                  - Corresponding constant
                  - Description of the situation
                * - ``"start"``
                  - ``$.sys.FAILED_AT_START``
                  - Couldn't start the program.
                * - ``"write"``
                  - ``$.sys.FAILED_AT_WRITE``
                  - Error in writing data to stdin.
                    If **crashed** = ``false``, then the time allotted for writing data to stdin has expired.
                * - ``"finish"``
                  - ``$.sys.FAILED_AT_FINISH``
                  - Error pending completion of the program.
                    If **crashed** = ``false``, it means that the program has not been completed in time.

            If **opts.detached** = ``true``, then this parameter can only contain the value ``$.sys.FAILED_AT_START``.

        * stdout: String
            Contents of the standard program output (stdin), represented as a string in the UTF-8 encoding.
            Absent if **opts.detached** = ``true``.

        * stderr: String
            Contents of the standard program error output (stderr), represented as a string in the UTF-8 encoding.
            Absent if **opts.detached** = ``true``.

        * exitCode: Number
            Program exit code.
            Absent if **opts.detached** = ``true``.

    *Example:*

    .. code-block:: javascript

        // execute the program and show the result
        console.log($.sys.exec('fortune').stdout)
        
        // execute the program and check if there's any startup error
        if($.sys.exec('ls').failedAt == $.sys.FAILED_AT_START)
            console.log('Program failed to start')
        
        // execute the program by passing the data into stdin and additional arguments
        var result = $.sys.exec(['cowsay', '-t'], {
            stdin: $.dev.name,
            timeout: 1
        })
        console.log(
            result.exitCode === 0
                ? result.stdout
                : 'Error: ' + result.failedAt + ': ' + result.stderr
        )


.. _script-ref-tools:

Helper functions
----------------

* **$.tools.fromColor(color: ColorString) : ColorObject**
* **$.tools.fromColorF(color: ColorString) : ColorObject**
* **$.tools.toColor(color: ColorObject) : ColorString**
* **$.tools.toColorF(color: ColorObject) : ColorString**
    Convert a string containing color information (**ColorString** type) into an object containing information on individual RGBA color components (**ColorObject** type) and vice versa. The suffix "F" means that **ColorObject** contains data in normalized form (from 0 to 1 inclusive). Functions without this suffix imply that each RGBA color component contains values in the range from 0 to 255 inclusive. Type **ColorString** can be a string in the following formats: **#rrggbb** (e.g. ``"#ff9900"``), **#aarrggbb** (e.g. ``"#eeff9900"``) or string color representation (e.g. ``"lightblue"``). The **toColor** and **toColorF** functions return the color in **#rrggbb** format if the color is not transparent (alpha channel has a maximum value). If transparency is available, these functions return the color in **#aarrggbb** format.

    *Example:*

    .. code-block:: javascript

        var cStr = $.dev.colorTempWarning // #ff9900
        var cObj = $.tools.toColorF(cStr) // {r: 1, g: 0.6, b: 0, a: 1}
        cObj.g = 0.2
        cObj.a = 0.25
        cStr = $.tools.fromColorF(cObj) // #40809900
        $.dev.colorTempWarning = cStr
        
        cStr = 'lightblue'
        cObj = $.tools.toColor(cStr) // {r: 173, g: 216, b: 230, a: 255}
        cStr = $.tools.fromColor(cObj) // #add8e6


.. _script-ref-tools-clone:

* $.tools.clone(obj: Object, deep: Boolean = false) : Object
    Create a copy of the object **obj**.
    If **deep** = ``true``, then recursively create copies of all nested properties of the object.
    If **deep** = ``false``, the object properties are copied by simple assignment.

    *Example:*

    .. code-block:: javascript

        var obj = {a: {}}
        
        var shallowCopy = $.tools.clone(obj)
        obj === shallowCopy // false
        obj.a === shallowCopy.a // true
        
        var deepCopy = $.tools.clone(obj, true)
        obj === shallowCopy // false
        obj.a === shallowCopy.a // false


.. |rw| replace:: :superscript:`read/write`
.. |T1| replace:: T\ :sub:`1`
