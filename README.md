# rpiMySensorsIO

Project goal is to have JSON based configuration file which specifies how IO pins on (currently only RPI supported) MySensors platform will behave.

## config.json
### switches
Are the abstract configuration unit that specifies how output pin will behave.
#### example:
```json
"switches": {
    "out1": {
        "mode": 0,
        "impulseDuration": 500,
        "outPin": 11,
        "present": true
    }
}
```
#### description:
**out1** - name of the switch. Send to MySensors cotroller if **present** is true.

**mode**:
* 0 : **(default)** standard mode if the switch is activated then output pin is set to state 1 until switch is active.
* 1 : impuse mode means that if the switch is activated or deactivated output pin will do impulse from 0 to 1 and back to 0 for **impulseDuration** specified time period

**impulseDuration** - **(default: 300)** only applyed when **mode** is 1 (impulse) and its value specifies how long will impulse last

**outPin** - HW IO pin which will be used as output pin

**present** - **(default: true)** boolean property which specify if this switch will be presented (usable) in MySensors controller

### inputs
Imputs related suff
#### config
Shared configuration for input elements.
##### example:
```json
"config": {
    "debounce": 50,
    "multiMaxDelay": 800
}
```
##### description:
**debounce** - debounce interval in ms

**multiMaxDelay** - maximum period in ms during which next press is counted into multipres signal

#### inputPins
Abstract configuration unit which specifes some properties of how input pin should behave.
Could ve specified here or will be created by trigger with defalut parameters.
##### example:
```json
"inputPins": {
    "inPin7": {
        "pin": 7,
        "present": true
    }
}
```
##### description:
**inPin7** - name of the input pin. Send to MySensors cotroller if **present** is true.

**pin** - HW pin which will be used **WARNING** internal pullup is used. If pin driven with buton put other end of button against ground

**present** - **(default: true)** boolean property which specify if this input pin will be presented (usable) in MySensors controller

#### triggers
Abstract configuration unit which can specify how input actions are transformed to output.
##### example:
```json
"vtl1": {
    "inPin": 7,
    "event": 3,
    "count": 1,
    "drives": [
        {
            "switch": "out2",
            "action": 2
        }
    ]
}
```
##### description:
**vtl1** - name of the trigger. Not used for presentation.

**inPin** - input pin used for this trigger. If input pin behavior is specified in **inputPin** section then that one is used. If not default one is created. Mapping is done by HW pin number.

**event** - specifies which input pin event should this trigger listen to
* **0 (down)** - the input pin sate goes to down
* **1 (up)** - **(default)** the input pin state goes to up
* **2 (long)** - the input pin state goes to long which means that input pin is activated for period more then **multiMaxDelay** specifies
* **3 (multi)** - the input pin was pressed by property **"count"** specified times. 

**count** - applies when trigger evetn multi is used and specifies count of pin activations to activate trigger

**drives** - array of objects which are driven by this trigger
* **switch** - name of the switch which is driven by this trigger
* **action** - specifies what shoud switch do after this trigger activation
  * **0 (toggle)** - **(default)** switch is toggled
  * **1 (up)** - switch is activated
  * **2 (down)** - switch is deactivated

## Future thoughts / ideas

### MySensors controller uploadable config
Feature that will allow to upload configuration over MySensors network from MySensors controller to RPI.
Started not finished yet.

### SPI1 connected arduino boards
Feature that will allow to extend IO whit aditional aruino board connected via SPI bus.