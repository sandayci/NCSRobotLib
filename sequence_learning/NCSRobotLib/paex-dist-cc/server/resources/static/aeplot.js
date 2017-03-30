
var plotNumCells = 256;

var plot2d = false;
var plotraster = false;


//---2d plot---

function gridCreate(numx,numy,gridid=0) {
    var width = numy * 22; //22px wide cells
    var height = numx * 22;
    var div = document.getElementById('grid' + gridid);

    div.style.width = width + 'px';
    div.style.height = width + 'px';

    var cellid = 0;
    for (var i = 0; i < numy; i++) {
        for (var p = 0; p < numx; p++) {
            var cell = document.createElement('div');
            cell.id = 'grid_' + gridid + '_' + cellid;
            cell.virtid = cellid;
            cell.spikecount = 0;
            var info = document.createElement('span');
            info.innerHTML = 'cell id ' + cellid + ': <strong>0 Hz</strong>';
            info.id = cell.id + '_info';
            cell.appendChild(info);
            div.appendChild(cell);
            cellid++;
        }
    }
}

function fadeCell(id) {
    var cell = document.getElementById(id);
    var val = 255 * cell.val;
    var rgb = 'rgb(' + val + ',' + val + ',' + val + ')';
    cell.active = true;
    cell.style.background = rgb;
    if (cell.val < 1) {
        cell.val++;
        setTimeout(function(){ fadeCell(id); }, 10);
    }
    else{
        cell.active = false;
        return;
    }
}

function triggerCell(id) {
    var cell = document.getElementById(id);
    cell.val = 0;
    cell.spikecount++;
    if (cell.active) {
        return;
    }
    else {
        fadeCell(id);
    }
}

function infoUpdate(gridid=0) {
    for (var i = 0; i < plotNumCells; i++) {
        var id = 'grid_' + gridid + '_' + i;
        var cell = document.getElementById(id);
        var info = cell.getElementsByTagName('span')[0];
        var rate = info.getElementsByTagName('strong')[0];
        rate.innerHTML = cell.spikecount + ' Hz';
        cell.spikecount = 0;
    }
}

function gridUpdate(events) {
    for (var i = 0; i < events.length; i++ ){
            triggerCell('grid_0_' + h2d(events[i]));
            el_raw_event.textContent = '0x'+events[i];
    }
}

function gridInit() {
    //set up grid 0
    gridCreate(16,16);
    //init regular updates of cell info
    setInterval(infoUpdate, 1000);
    plot2d = true;
}


//---raster plot---

function binsizeSelector() {
	var select = document.createElement('select');
        var choices = [2,5,10,20,50,100,200,500];
	var selected = 10;
	for (i = 0; i < choices.length; i++) {
		var option = document.createElement('option');
		option.value = choices[i];
		option.innerHTML = choices[i]+" ms";
		if (choices[i] == selected)
			option.selected = true;
		select.appendChild(option);
	}
	return select;
}

function binsizeUpdate(el) {
	var RPCStr = jsonRPCStr("setBinsize", {'ms': parseInt(el.value)});
	socket_di.send(RPCStr);
}

function rasterInit() {
    var div_cvs = document.getElementById("raster0");
    var div_ctl = document.getElementById("ctl");
    var canvas = document.createElement('canvas');
    canvas.id = "raster_canvas";
    canvas.width = 360;
    canvas.height = plotNumCells;
    canvas.state = 0;
    canvas.rasterStyle = [];
    for (var i=0; i<plotNumCells; i++)
        canvas.rasterStyle.push('#000');
    div_cvs.appendChild(canvas);
    var bsSel = binsizeSelector()
    bsSel.addEventListener('change', function(){binsizeUpdate(this);});
    bsSel.addEventListener('keyup', function(){binsizeUpdate(this);});
    div_ctl.appendChild(bsSel);
    plotraster = true;
}

function rasterUpdate(events) {
    var canvas = document.getElementById('raster_canvas');
    var ctx = canvas.getContext('2d');
    //plot
    if (canvas.state == canvas.width-1)
        clearCanvas(canvas);
    for (var i=0; i < events.length; i++) {
        n = h2d(events[i]);
	ctx.fillStyle = canvas.rasterStyle[n];
        ctx.fillRect(canvas.state, canvas.height-1-n, 1, 1);
        el_raw_event.textContent = '0x'+events[i];
    }
    canvas.state ++;
}

function rasterSetStyle(n, style) {
    var canvas = document.getElementById('raster_canvas');
    canvas.rasterStyle[n] = style;
}

function clearCanvas(canvas) {
    var ctx = canvas.getContext('2d');
    canvas.state = 0;
    ctx.clearRect(0, 0, canvas.width, canvas.height);
}


//---common---

function plotUpdate(events) {
    if(plot2d) gridUpdate(events);
    if(plotraster) rasterUpdate(events);
}
