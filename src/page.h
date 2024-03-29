const char webpageCode[] PROGMEM= 
R"=====(
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Auto-Starter</title>
    <style>
        body {
            display: flex;
            flex-direction: column;
            justify-content: center;
            align-items: center;
            background-color: #222222;
            border: 1px solid #07ffcdcd;
            border-radius: 10px;
            box-shadow: 0 0 15px #07ffcdcd;
            margin: 2vw;
            padding: 2vw;
        }

        hr {
            border: 1px solid #07ffcd3e;
            width: 100%;
            margin: 1rem;

        }

        heading {
            font-size: 2rem;
            color: #fff;
            text-shadow: 0 0 15px #07ffcdcd;
            font-family: 'Lobster', cursive;
        }

        time {
            display: flex;
            flex-direction: column;
            justify-content: center;
            align-items: center;
        }

        time input {
            width: 35vw;
            border: 1px solid #07ffcdcd;
            border-radius: 10px;
            background-color: #222222;
            color: #fff;
            text-shadow: 0 0 15px #07ffcdcd;
            font-family: 'Lobster', cursive;
            text-align: center;
            margin: 1rem;
            padding: 1rem;
        }

        time input[type="time"] {
            font-size: 1.5rem;
        }

        main_switch {
            display: flex;
            flex-direction: row;
            justify-content: center;
            align-items: center;
        }

        .switch {
            display: inline-block;
            width: 4rem;
            height: 2rem;
            margin: 1rem;
            border-radius: 100px;
            position: relative;
            background-color: #07ffcdcd;
            box-shadow: 0 0 15px #07ffcdcd;
            cursor: pointer;
        }

        .switch .slider {
            position: absolute;
            top: 0;
            left: 0;
            height: 2rem;
            width: 2rem;
            border-radius: 50%;
            background-color: #222222;
            transition: 0.3s;
        }

        .switch input {
            display: none;
        }

        .switch input:checked+.slider {
            transform: translateX(100%);
        }


        #status_light {
            width: 1rem;
            height: 1rem;
            border-radius: 50%;
            background-color: red;
            margin-left: 0.5rem;
        }

        schedules_list {
            display: flex;
            flex-direction: column;
            justify-content: center;
            align-items: center;
        }

        schedules_list h1,
        schedules_list h2 {
            font-size: 1.5rem;
            color: #fff;
            text-shadow: 0 0 15px rgba(7, 255, 205, 0.8);
            font-family: 'Lobster', cursive;
            margin: 1rem;
        }

        schedules_list .schd {
            display: flex;
            flex-direction: row;
            justify-content: center;
            align-items: center;
            margin: 1rem;
            padding: 1rem;
            border: 1px solid rgba(7, 255, 205, 0.8);
            border-radius: 10px;
            background-color: #222222;
            box-shadow: 0 0 15px rgba(7, 255, 205, 0.8);
        }

        schedules_list .start,
        schedules_list .end {
            display: flex;
            flex-direction: column;
            align-items: center;
        }

        schedules_list .start h2,
        schedules_list .end h2 {
            margin-bottom: 0.5rem;
        }

        schedules_list p {
            color: #fff;
            font-size: 1rem;
            margin: 0.5rem 0;
        }
    </style>
</head>

<body>
    <heading>
        Auto-Starter
        <main_switch>
            <label class="switch">
                <input type="checkbox" id="toggle">
                <span class="slider"></span>
            </label>
            <div id="status_light"></div>
        </main_switch>
    </heading>
    <hr>

    <time>
        <input type="time" name="start" id="start" required step="300">
        <input type="time" name="end" id="end" required step="300">
        <op>
            <input type="button" value="ADD" id="add">
            <input type="button" value="REMOVE" id="remove">
        </op>
        <input type="button" value="RESET" id="reset">
    </time>
    <hr>

    <schedules_list>
        <h1>
            Schedules
        </h1>
        <div class="schd">
            <div class="start">
                <h2>Start</h2>
            </div>
            <div class="end">
                <h2>End</h2>
            </div>
        </div>
    </schedules_list>
</body>
<script>
    async function getData() {
        const response = await fetch("/data");
        const data = await response.json();
        return data
    }
    async function load() {
        getData().then(function (data) {
            console.log(data);
            var status = data.status;
            if (status == "true") {
                toggle.checked = true;
                statusLight.style.backgroundColor = "green";
            } else {
                toggle.checked = false;
                statusLight.style.backgroundColor = "red";
            }
        });
    }

    async function changeStatus(status) {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function () {
            if (this.readyState == 4 && this.status == 200) {
            }
        };
        xhttp.open(
            "GET",
            "/?status=" +
            status,
            true
        );
        xhttp.send();
        load();
    }



    setInterval(async function () {
        getData().then(function (data) {
            var status = data.status;
            if (status == "true") {
                toggle.checked = true;
                statusLight.style.backgroundColor = "green";
            } else {
                toggle.checked = false;
                statusLight.style.backgroundColor = "red";
            }

            for (i = 0; i < data.start_time_list.length; i++) {
                insert(data.start_time_list[i], data.end_time_list[i]);
            }

        });
    }, 2500);

</script>
<script>
    const toggle = document.getElementById("toggle");
    const statusLight = document.getElementById("status_light");
    toggle.addEventListener("change", function () {
        if (toggle.checked) {
            changeStatus("true");
            statusLight.style.backgroundColor = "green";
        } else {
            changeStatus("false");
            statusLight.style.backgroundColor = "red";
        }
    });

    const add_btn = document.getElementById("add");
    const remove_btn = document.getElementById("remove");

    add_btn.addEventListener("click", function () {
        var start_time = formatTime(document.getElementById("start").value);
        var end_time = formatTime(document.getElementById("end").value);
        if ((start_time != "" || end_time != "") && (start_time != end_time)) {
            send(start_time, end_time, "add");
        }
    });

    remove_btn.addEventListener("click", function () {
        var start_time = formatTime(document.getElementById("start").value);
        var end_time = formatTime(document.getElementById("end").value);
        if (start_time != "" || end_time != "") {
            send(start_time, end_time, "remove");
            refresh();
        }
    });

    document.getElementById("reset").addEventListener("click", function () {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function () {
            if (this.readyState == 4 && this.status == 200) { }
        };
        xhttp.open("GET", "/reset", true);
        xhttp.send();
        refresh();
    });

    function formatTime(timeInput) {
        const [hours, minutes] = timeInput.split(":");
        let parsedHours = parseInt(hours, 10);
        const amPm = parsedHours >= 12 ? "PM" : "AM";
        parsedHours = parsedHours % 12 || 12;
        const formattedHours = parsedHours.toString().padStart(2, '0');
        return `${formattedHours}:${minutes} ${amPm}`;
    }

    function isAlreadyExist(start_time, end_time) {
        var start = document.getElementsByClassName("start")[0];
        var end = document.getElementsByClassName("end")[0];
        var start_p = start.getElementsByTagName("p");
        var end_p = end.getElementsByTagName("p");
        for (var i = 0; i < start_p.length; i++) {
            if (start_p[i].innerHTML == start_time) {
                return true;
            }
        }
        for (var i = 0; i < end_p.length; i++) {
            if (end_p[i].innerHTML == end_time) {
                return true;
            }
        }
        return false;
    }

    function insert(start_time, end_time) {
        if (!isAlreadyExist(start_time, end_time)) {
            var start = document.getElementsByClassName("start")[0];
            var end = document.getElementsByClassName("end")[0];
            var start_p = document.createElement("p");
            var end_p = document.createElement("p");
            start_p.innerHTML = start_time;
            end_p.innerHTML = end_time;
            start.appendChild(start_p);
            end.appendChild(end_p);
        }
    }

    function remove(start_time, end_time) {
        var start = document.getElementsByClassName("start")[0];
        var end = document.getElementsByClassName("end")[0];
        var start_p = start.getElementsByTagName("p");
        for (var i = 0; i < start_p.length; i++) {
            if (start_p[i].innerHTML == start_time) {
                start.removeChild(start_p[i]);
            }
        }
        var end_p = end.getElementsByTagName("p");
        for (var i = 0; i < end_p.length; i++) {
            if (end_p[i].innerHTML == end_time) {
                end.removeChild(end_p[i]);
            }
        }
    }

    function send(start_time, end_time, op_code) {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function () {
            if (this.readyState == 4 && this.status == 200) {
            }
        };
        xhttp.open(
            "GET",
            "/?op=" +
            op_code +
            "&start=" +
            start_time +
            "&end=" +
            end_time,
            true
        );
        xhttp.send();
    }

    function refresh(){
        setTimeout(function(){
            window.location.reload();
        }, 2500);
    }

</script>

</html>
)=====";
