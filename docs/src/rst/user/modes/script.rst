Script
======

The "Script" mode allows you to set an arbitrary algorithm for calculating the required speed of the video card fans. Every second information about the current state of the video card, including its temperature, will be transmitted to the script\ [#script]_ . Based on this information, the script can make one of the following decisions:

* Set the required fan speed.
* Let the video card determine the desired fan speed.


.. note:: The actual fan speed may not match exactly what is shown on the graph. This depends on the video card. For example, if the video card does not support low fan speeds, the actual fan speed may be 30% when set to 5%.


The algorithm is written in ECMAScript 6\ [#javascript]_ language.

.. _mode-script-mainFunction:

The source code must contain the main function that will be called every second. This function must be the last function to be called by the script operator. This can be done in two ways:

* Implement the main function as an arrow function\ [#arrow-function]_ and put it at the end of the script.

.. code-block:: javascript

    <code before the main function>
    
    ()=>{
        <the main function code>
    }
    // end of the script

* Place the function identifier as the last script operator.

.. code-block:: javascript

    function f1() {
        <code of the first function>
    }
    
    function f2() {
        <code of the second function>
    }
    
    // choosing the main function depending on the plugin category
    $.dev.plugin.category == 'nvidia' ? f1 : f2
    // end of the script


The main function will only be executed if the script is :ref:`running <mode-script-status-running>`. Below is an example of the script that sets the fan speed if the temperature has risen above 60°C, and if the temperature is below 60°C, the script gives the fan control to the video card.

.. code-block:: javascript

    // When the script is launched for the first time, 
    // the code that is located outside of the main function
    // will be executed.
    // Here you can initialize the variables
    // and define additional functions.

    var tempThreshold = 60
    
    function getTimestamp()
    {
        return Math.round(Date.now() / 1000)
    }
    
    
    // Next comes the main function that will be called every second.
    ()=>{
        var secsPassed = getTimestamp() - tsStart
        if(secsPassed && secsPassed % 10 == 0)
            console.log('The script is running for '+secsPassed+' seconds')
    
        // $.dev.curTemp - current video card temperature
        if($.dev.curTemp > tempThreshold)
            return $.dev.curTemp / 2 // set the fan speed
        else
            return $.dev.FAN_SPEED_GPU // let the video card determine the fan speed
    }

It is recommended to read the following information before writing the script:

.. toctree::
    :maxdepth: 1
    :hidden:

    script/reference

* :doc:`script/reference`.
* The list of available built-in ECMAScript objects: `<https://doc.qt.io/qt-5.12/ecmascript.html>`_.


Interface description of the "Script" mode window
-------------------------------------------------

.. _mode-script-editor:

* Code editor
    Code editor with minimal functionality and syntax highlighting. In addition to text editing, it allows you to perform the following actions:

        * "CTRL + mouse wheel" adjusts the size of the text.
        * The Tab key can move selected text block to the right.
        * Shift+Tab can move selected text block to the left.

    It is important to remember that the source code displayed in the editor may not match the source code of the currently running script.
    Changing the source code in the editor will not affect the already running script.
    To start the script with the new source code, you should press the "Apply and restart" button.


* Script status
    May contain several comma-separated values. Possible values:

    .. _mode-script-status-running:

    * **started** - the script is running and the source code displayed in the editor corresponds to the source code of the running script.

    * **running old script** - the script is running, but the source code that is displayed in the editor has been changed since the start of the script and no longer matches the source code of the running script.

    .. _mode-script-status-paused:

    * **paused** - the script was initialized, but is temporarily suspended. This may happen if the script is in the :ref:`"Override" mode <mode-script-override>`.

    * **old script paused** - same as the previous status, but the source code that is displayed in the editor, has been changed since the start of the script and no longer corresponds to the source code of the initialized script.

    .. _mode-script-status-stopped:

    * **stopped** - the script is not active and will not be executed.

    * **not saved** - the current source code displayed in the editor has not been saved to disk and will be lost when the program restarts.


.. _mode-script-override:

* Override
    If this switch is enabled, the script will only work when other modes are enabled (except ":doc:`gpu`" mode). Every second the internal algorithm of the selected mode will be executed, and then the script will be executed to further modify the resulting fan speed that has been set in that mode.

    For example, if you check the "Override" box, run the script and enable the ":doc:`curve`" mode, the following will happen every second:

    1. The required fan speed is calculated from the curve points and hysteresis

    2. This speed is stored in the :ref:`$.dev.targetFanSpeed <script-ref-dev-targetFanSpeed>` variable.

    3. The main function of the script is called. It can return either another desired fan speed or **$.dev.targetFanSpeed** (or :ref:`$.dev.FAN_SPEED_NO_CHANGE <script-ref-dev-fanSpeedNoChange>`) to leave the calculated speed as is.

    4. The main function return value will become the final target fan speed. The script can also return **$.dev.FAN_SPEED_GPU** to let the video card choose the fan speed.


    If "Override" is enabled, the script cannot work when "Script" mode is enabled. Therefore, in this case, it will be :ref:`paused <mode-script-status-paused>`. If "Override" is not enabled, the script will work only when the "Script" mode is selected, otherwise it will be paused. The script will also be paused if the ":doc:`gpu`" mode is selected. The script will be paused only if it has been started.


* Apply and restart
    Restarts the script with the new source code taken from the editor. After that the script will be executed every second (taking into account the :ref:`"Override" mode<mode-script-override>`). The sequence will be as follows:

        1. If the script was :ref:`running <mode-script-status-running>` or was :ref:`paused <mode-script-status-paused>` it will be :ref:`stopped <mode-script-status-paused>`.

        2. The source code in the code editor will be used as the source code for the new script.

        3. The script is initialized (running). The script must specify the :ref:`main function <mode-script-mainFunction>`.

        4. If the :ref:`"Override" <mode-script-override>` switch is set, the script will be :ref:`paused <mode-script-status-paused>`, otherwise it will be in the :ref:`"running" <mode-script-status-running>` state.


* Stop
    Stops the script.


.. rubric:: Notes

.. [#script] "Script" ("Scripting language") article in Wikipedia: `<https://en.wikipedia.org/wiki/Scripting_language>`_.
.. [#javascript] "ECMAScript" article in Wikipedia: `<https://en.wikipedia.org/wiki/ECMAScript>`_.
.. [#arrow-function] "Arrow functions" article in MDN: `<https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Functions/Arrow_functions>`_.
