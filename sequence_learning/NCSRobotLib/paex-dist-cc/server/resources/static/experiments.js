
var Exp = {'ready': false};

function experimentsInit() {
    get_json('_experiments.json', render_experiments_list);
}

//display list of files
function render_experiments_list(expList) {
    var div = document.getElementById("experiment").getElementsByTagName("div")[0];
    var list = document.createElement('ul');
    list.className = 'filelist';
    for (var i = 0; i < expList.files.length; i++) {
	var item = document.createElement('li');
	var a1 = document.createElement('a');
	var span = document.createElement('span');
        a1.addEventListener("click", function(){expLoad(this);});
	a1.innerHTML = expList.files[i];
	a1.href = '#';
	item.appendChild(a1);
	item.appendChild(span);
	list.appendChild(item);
    }
    div.appendChild(list);
}

function expAddButton(title, obj, action, singleuse=false) {
    button = document.createElement("input");
    button.type = "button";
    button.value = title;
    button.singleuse = singleuse;
    button.addEventListener("click", function(){
        obj[action](obj);
        if (button.singleuse)
            button.setAttribute("disabled","disabled");
    });
    expAddElement(button);
}

function expAddElement(el) {
    var div = document.getElementById("experiment").getElementsByTagName("div")[0];
    div.appendChild(el);
}

function drawEdge(ctx, frCoords, toCoords) {
    var d = Math.sqrt(Math.pow(frCoords.x-toCoords.x,2) + Math.pow(frCoords.y-toCoords.y,2));
    var color = 200-200/d;
    var opacity = 0.66/d + 0.33;
    ctx.strokeStyle = "rgba("+color+",200,200,"+opacity+")";
    ctx.beginPath();
    ctx.moveTo((frCoords.x+1)*50+5, (frCoords.y+1)*50+5);
    ctx.lineTo((toCoords.x+1)*50+5, (toCoords.y+1)*50+5);
    ctx.stroke();
}

function drawNode(ctx, coords) {
    ctx.fillStyle = '#000';
    ctx.fillRect((coords.x+1)*50, (coords.y+1)*50, 10, 10);
}

function drawLabel(ctx, coords, label) {
    ctx.fillStyle = '#000';
    ctx.font = "10px sans";
    ctx.fillText(label, (coords.x+1)*50+10, (coords.y+1)*50+20);
}

function drawPixel(ctx, coords, w, scale=1, offset=11) {
    if (w < 0)
	ctx.fillStyle = "rgba(200,000,000,1)";
    else
	ctx.fillStyle = "rgba(000,000,200,1)";
    ctx.fillRect((offset+coords.x)*scale,(offset+coords.y)*scale, scale, scale);
}

function connectCells(source, target, type, w) {
    var w1opts = [0, 1, 0, 1];
    var w2opts = [0, 0, 1, 1];
    var params = {};
    params.connSource = source;
    params.connTarget = target;
    params.connType = (type == "exc") ? 1 : 0;
    params.connWgt1 = w1opts[w];
    params.connWgt2 = w2opts[w];
    RPCStr = jsonRPCStr("setConn", params);
    socket_di.send(RPCStr);
}

//pass events to experiment code
function expUpdate(events) {
    if (Exp.ready && (Exp.getNext || Exp.getAuto)) {
	Exp.getNext = false;
        var sample = []
        for (var i=0; i < events.length; i++)
           sample[i] = h2d(events[i]);
        Exp.update(sample);
    }        
}

function expResetDiv() {
    var div = document.getElementById("experiment").getElementsByTagName("div")[0];
    while (div.firstChild) {
        div.removeChild(div.firstChild);
    }
}

function expLoad(a) {
    var head = document.getElementsByTagName('head')[0];
    var script = document.createElement('script');
    script.type = 'text/javascript';
    script.onload = function() { expInit(); };
    script.src = a.innerHTML;
    head.appendChild(script);
}

//this is run once the experiment js is loaded
function expInit() {
    expResetDiv();
    var Exp = new Experiment();
    var a = document.getElementById("experiment").getElementsByTagName("a")[0];
    a.innerHTML = Exp.title;
}

