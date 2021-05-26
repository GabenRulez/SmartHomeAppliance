#ifndef websiteFile
#define websiteFile

const char HTML[] PROGMEM = "<!doctypehtml><meta charset=UTF-8><meta content=\"width=device-width,initial-scale=1\"name=viewport><title>Smart Home Appliance</title><link href=https://fonts.gstatic.com rel=preconnect><link href=\"https://fonts.googleapis.com/css2?family=Open+Sans&display=swap\"rel=stylesheet><link href=https://cdn.jsdelivr.net/npm/reinvented-color-wheel@0.3.1/css/reinvented-color-wheel.min.css rel=stylesheet><style>:root{--options:5;--white:#FAF3DD;--black:#170F11;--light-grey:#5C6B73;--dark-grey:#404A4F;--orange:#FF9505}html{background:var(--black)}*{font-family:\"Open Sans\",sans-serif;color:var(--white);margin:0;padding:0;box-sizing:border-box;overflow-x:hidden;text-align:center;transition:1s ease}#indicator{display:block;width:2vmin;height:2vmin;top:2vmin;left:2vmin;position:absolute;border-radius:9999px;background:var(--dark-grey)}.offline{background:red!important}.online{background:green!important}h1{font-size:3em}.line{width:100%;border-bottom:1px solid var(--light-grey)}.flex-column{display:flex;flex-direction:column;align-items:center;justify-content:center;gap:1em}.flex-row{display:flex;flex-direction:row;align-items:center;justify-content:center;gap:1em}#navbar{display:grid;height:calc(10vh - .5vmin);margin-top:.5vmin;justify-content:center;grid-template-columns:repeat(var(--options),calc(100% / (var(--options) + 2)));column-gap:1vmin;width:100%}.option{border:2px solid var(--light-grey);border-radius:1vmin;background:var(--dark-grey)}.option.active{border-color:var(--orange)}#navbar>.option{display:flex;align-items:center;justify-content:center}#container{display:grid;width:calc(var(--options) * 100%);min-height:90vh;padding:1vmin;grid-template-columns:repeat(var(--options),80vw);column-gap:5vw;transform:translateX(10vw)}#container>.option{box-shadow:0 0 1vmin var(--light-grey);padding:5% 10%;display:flex;align-items:center;justify-content:space-around;flex-direction:column}#container>.option.active{box-shadow:0 0 1vmin var(--orange)}.button{border:2px solid var(--light-grey);border-radius:1vmin;padding:1vmin;cursor:pointer}.button.disabled{cursor:not-allowed;box-shadow:inset 0 0 5000px var(--light-grey)}#rainbowSpeed,#rainbowStrength,#twoColorsSpeed,#warmLightRange{width:80%}.reinvented-color-wheel div,canvas{transition:0s}.realResult{display:none}@media screen and (max-width:550px){.flex-row{flex-direction:column}}</style><div id=indicator></div><div id=navbar><div class=\"option active welcome\">Home screen</div><div class=\"option warmLight\">Warm Lights</div><div class=\"option rgb\">Static RGB Mode</div><div class=\"option twoColors\">Two color transition</div><div class=\"option rainbow\">Rainbow</div></div><div id=container><div class=\"option active welcome\"><h1>Smart Home Appliance</h1><div class=flex-column><h3>This is a control menu of Smart Home Appliance. You can read more about this product here:</h3><a href=https://github.com/GabenRulez/SmartHomeAppliance>GitHub.com/GabenRulez/SmartHomeAppliance</a></div><div class=line></div><h2>Quick Guide</h2><div class=flex-column><p>You can navigate through different modes by selecting one of the blocks on top of the page.<p>You can also do the same by clicking on the parts of the sections on either side.<div class=line style=width:50%></div><p>Once you select one of the modes, you can choose certain parameters and then press \"RUN\".<p>ESP32 will then receive a message and act accordingly.</div></div><div class=\"option warmLight\"><h1>Warm Lights</h1><div class=flex-column style=width:80%><h2>Brightness</h2><input autocomplete=off id=warmLightRange max=255 min=0 name=warmLightRange oninput='this.parentNode.querySelector(\"h2.realResult\").innerHTML=this.value,this.parentNode.querySelector(\"h2.result\").innerHTML=Math.floor(100*this.value/255)+\"%\"'type=range value=128><h2 class=result>50%</h2><h2 class=realResult>128</h2></div><div class=flex-row><div class=button data-value=warmLights>Turn \"Warm Lights\" mode ON</div><div class=button data-value=modeOFF>Turn OFF</div></div><div class=line></div><div class=flex-column><p>This mode turns on the 12V warm-white LEDs.<p>Recommended for hours before sleep.</div></div><div class=\"option rgb\"><h1>RGB Lights</h1><div class=\"flex-column rgbPicker\"id=rgbPickerContainer><div class=redValue style=display:none>255</div><div class=greenValue style=display:none>0</div><div class=blueValue style=display:none>0</div></div><div class=flex-row><div class=button data-value=RGBLights>Turn \"RGB Lights\" mode ON</div><div class=button data-value=modeOFF>Turn OFF</div></div><div class=line></div><div class=flex-column><p>This mode turns on the 5V addressable RGB LEDs.<p>They help bring \"the mood\" to the room.</div></div><div class=\"option twoColors\"><h1>Two color transition</h1><div class=flex-row><div class=\"flex-column rgbPicker\"id=rgb1PickerContainer><div class=redValue style=display:none>255</div><div class=greenValue style=display:none>0</div><div class=blueValue style=display:none>0</div></div><div class=\"flex-column rgbPicker\"id=rgb2PickerContainer><div class=redValue style=display:none>255</div><div class=greenValue style=display:none>0</div><div class=blueValue style=display:none>0</div></div></div><div class=flex-column style=width:80% id=twoColorsSpeedContainer><h2>Speed</h2><input autocomplete=off id=twoColorsSpeed max=255 min=1 name=twoColorsSpeed oninput='this.parentNode.querySelector(\"h2.realResult\").innerHTML=256-this.value,this.parentNode.querySelector(\"h2.result\").innerHTML=formatNumber(128/(256-this.value),2)+\"x\"'type=range value=127><h2 class=result>1.00x</h2><h2 class=realResult>127</h2></div><div class=flex-row><div class=button data-value=twoColors>Turn two color smooth transition mode ON</div><div class=button data-value=modeOFF>Turn OFF</div></div><div class=line></div><div class=flex-column><p>This mode turns on the 5V addressable RGB LEDs.<p>It then proceeds to smoothly transition between the two.<p>They help bring \"the mood\" to the room.</div></div><div class=\"option rainbow\"><h1>Rainbow</h1><div class=flex-column style=width:80% id=rainbowStrengthContainer><h2>Brightness</h2><input autocomplete=off id=rainbowStrength max=255 min=0 name=warmLightRange oninput='this.parentNode.querySelector(\"h2.realResult\").innerHTML=this.value,this.parentNode.querySelector(\"h2.result\").innerHTML=Math.floor(100*this.value/255)+\"%\"'type=range value=128><h2 class=result>50%</h2><h2 class=realResult>128</h2></div><div class=flex-column style=width:80% id=rainbowSpeedContainer><h2>Speed</h2><input autocomplete=off id=rainbowSpeed max=250 min=1 name=twoColorsSpeed oninput='this.parentNode.querySelector(\"h2.realResult\").innerHTML=256-this.value,this.parentNode.querySelector(\"h2.result\").innerHTML=formatNumber(128/(256-this.value),2)+\"x\"'type=range value=127><h2 class=result>1.00x</h2><h2 class=realResult>127</h2></div><div class=flex-row><div class=button data-value=rainbow>Turn rainbow loop mode ON</div><div class=button data-value=modeOFF>Turn OFF</div></div><div class=line></div><div class=flex-column><p>This mode turns on the 5V addressable RGB LEDs.<p>It then uses looped animation. The colors are basically spinning.<p>Gives you an extra 1fps in games.</div></div></div><script>const serverIP = \"192.168.0.102\";       // TODO delete later, as it will be source (default) ip\n        const containerObject = document.getElementById(\"container\");\n\n        for(let object of document.getElementsByClassName(\"option\")){\n            object.addEventListener('click', ()=>{\n                setNewActive(object.classList)\n            })\n        }\n\n        function setNewActive(classList){\n            if(!classList.contains(\"active\")){\n                let activeElements = document.getElementsByClassName(\"active\")\n                for(let i=activeElements.length-1; i>=0; i--) activeElements[i].classList.remove(\"active\");\n\n                let properClassName;\n                for(let className of classList) {\n                    if(className !== \"option\" && className !== \"active\") {\n                        properClassName = className;\n                        break;\n                    }\n                }\n\n                for(let object of document.getElementsByClassName(properClassName)){\n                    object.classList.add(\"active\")\n                }\n\n                let orderNumber;\n                let objectsInContainer = containerObject.getElementsByClassName(\"option\");\n                for(let i=0; i<objectsInContainer.length; i++){\n                    if(objectsInContainer[i].classList.contains(properClassName)){\n                        orderNumber = i;\n                        break;\n                    }\n                }\n\n                containerObject.style.transform = \"translateX(\" + calculateTranslate(orderNumber) + \")\";\n            }\n        }\n        function calculateTranslate(number){\n            return String((-85) * Number(number) + 10) + \"vw\";\n        }\n\n\n        const indicatorCircle = document.getElementById('indicator');\n        function checkIfOnline(){\n            var request = new XMLHttpRequest();\n            request.open('GET', \"http://\" + serverIP + \"/alive\", true); // TODO Source will be serverIP, so there is no need for http://serverIP\n            request.timeout = 1000;\n            request.onload = function (){\n                indicatorCircle.classList.remove(\"offline\");\n                indicatorCircle.classList.add(\"online\");\n                request.ontimeout = function (){};\n            }\n            request.ontimeout = function (){\n                indicatorCircle.classList.remove(\"online\");\n                indicatorCircle.classList.add(\"offline\");\n            }\n            request.send(null);\n        }\n        window.setInterval(checkIfOnline, 5000);\n\n        for(let buttonElement of document.getElementsByClassName(\"button\")){\n            buttonElement.addEventListener('click', ()=>{\n                buttonElement.classList.add('disabled');\n                buttonElement.style.borderColor = \"\";\n                var request = new XMLHttpRequest();\n                switch(buttonElement.dataset.value){\n                    case \"warmLights\":\n                        request.open('GET', \"http://\" + serverIP + \"/warmLights?strength=\" + document.getElementById('warmLightRange').value, true);\n                        break;\n                    case \"RGBLights\":\n                        const parentObject = document.getElementById('rgbPickerContainer');\n                        request.open('GET', \"http://\" + serverIP + \"/RGBLights?red=\" + parentObject.querySelector('.redValue').innerHTML + \"&green=\" + parentObject.querySelector('.greenValue').innerHTML + \"&blue=\" + parentObject.querySelector('.blueValue').innerHTML, true);\n                        break;\n                    case \"twoColors\":\n                        const parentObject1 = document.getElementById('rgb1PickerContainer');\n                        const parentObject2 = document.getElementById('rgb2PickerContainer');\n                        request.open('GET', \"http://\" + serverIP + \"/twoColors?red1=\" + parentObject1.querySelector('.redValue').innerHTML + \"&green1=\" + parentObject1.querySelector('.greenValue').innerHTML + \"&blue1=\" + parentObject1.querySelector('.blueValue').innerHTML + \"&red2=\" + parentObject2.querySelector('.redValue').innerHTML + \"&green2=\" + parentObject2.querySelector('.greenValue').innerHTML + \"&blue2=\" + parentObject2.querySelector('.blueValue').innerHTML + \"&speed=\" + document.querySelector('#twoColorsSpeedContainer').querySelector('.realResult').innerText)\n                        break;\n                    case \"rainbow\":\n                        request.open('GET', \"http://\" + serverIP + \"/rainbow?strength=\" + document.querySelector('#rainbowStrength').value + \"&speed=\" + document.querySelector('#rainbowSpeedContainer').querySelector('.realResult').innerText)\n                        break;\n                    case \"modeOFF\":\n                    default:\n                        request.open('GET', \"http://\" + serverIP + \"/\" + buttonElement.dataset.value, true);\n                        break;\n                }\n                request.onload = function (){\n                    buttonElement.classList.remove('disabled');\n                    request.ontimeout = function (){};\n                }\n                request.ontimeout = function (){\n                    buttonElement.classList.remove('disabled');\n                    buttonElement.style.borderColor = \"red\";\n                }\n                request.send(null);\n            })\n        }\n        function formatNumber(number, digitsAfter){\n            if(digitsAfter === undefined) digitsAfter = 1;\n            let biggerNumber = Math.floor(number * (10 ** digitsAfter));\n            let tempString = String(biggerNumber);\n            for(let i=0; i<Math.log10(1.0/number); i++) tempString = \"0\" + tempString;\n            return [tempString.slice(0, tempString.length - digitsAfter), tempString.slice(tempString.length - digitsAfter)].join('.');\n        }</script><script type=module>import ReinventedColorWheel from \"https://cdn.jsdelivr.net/npm/reinvented-color-wheel@0.3.1/es/reinvented-color-wheel.bundle.min.js\";\n        /*\n        const redValueObject = document.getElementById('redValue');\n        const greenValueObject = document.getElementById('greenValue');\n        const blueValueObject = document.getElementById('blueValue');\n        const colorWheel = new ReinventedColorWheel({\n            appendTo: document.getElementById(\"rgbPickerContainer\"),\n            hsv: [0, 100, 100],\n            //wheelDiameter: 200,\n            //wheelThickness: 30,\n            //handleDiameter: 16,\n            wheelReflectsSaturation: true,\n            onChange: function (color) {\n                redValueObject.innerHTML = color.rgb[0];\n                greenValueObject.innerHTML = color.rgb[1];\n                blueValueObject.innerHTML = color.rgb[2];\n            },\n        });*/\n\n\n        for(let picker of document.getElementsByClassName('rgbPicker')){\n            const colorWheel = new ReinventedColorWheel({\n                appendTo: picker,\n                hsv: [0, 100, 100],\n                wheelReflectsSaturation: true,\n                onChange: function (color) {\n                    picker.querySelector('.redValue').innerHTML = color.rgb[0];\n                    picker.querySelector('.greenValue').innerHTML = color.rgb[1];\n                    picker.querySelector('.blueValue').innerHTML = color.rgb[2];\n                },\n            });\n        };</script>";

#endif
