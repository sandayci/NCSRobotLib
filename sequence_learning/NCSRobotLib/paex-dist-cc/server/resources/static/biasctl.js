
function coarseSelect(biasCoarseCurrents) {
	var coarseSelect = document.createElement('select');
        coarseSelect.className = "coarseCurrent";
	for (var cname in biasCoarseCurrents) {
		var option = document.createElement('option');
		option.value = biasCoarseCurrents[cname];
		option.innerHTML = cname;
		coarseSelect.appendChild(option);
	}
	return coarseSelect;
}

function fineSelect() {
	var fineSelect = document.createElement('select');
        fineSelect.className = "fineCurrent";
	for (i = 0; i < 256; i++) {
		var option = document.createElement('option');
		option.value = i;
		option.innerHTML = i;
		fineSelect.appendChild(option);
	}
	return fineSelect;
}

function init_bias_ctl(biasData) {
    var bias_tab_super = document.getElementById('biases_set');
    var bias_div_super = bias_tab_super.getElementsByTagName('div')[0];
    var tabs = document.createElement('div');
    tabs.className = 'tabs';
    for (var biasGroup in biasData.biasNames) { //loop through bias groups
        var bias_div_box = document.createElement('div');
        var bias_div = document.createElement('div');
        var bias_a = document.createElement('a');
        bias_a.innerHTML = biasGroup;
        bias_a.href = '#';
        bias_div_box.appendChild(bias_a);
        var list = document.createElement('ul');
        list.className = 'biaslist'
        for (var bname in biasData.biasNames[biasGroup]) { //loop through biases
            var item = document.createElement('li');
            var span = document.createElement('span');

            var coarseSel = coarseSelect(biasData.biasCoarseCurrents);
            coarseSel.addEventListener('change', function(){updateBias(this);});
            coarseSel.addEventListener('keyup', function(){updateBias(this);});

            var fineSel = fineSelect();
            fineSel.addEventListener('change', function(){updateBias(this);});
            fineSel.addEventListener('keyup', function(){updateBias(this);});

            var checkHigh = document.createElement('input');
            checkHigh.type = 'checkbox';
            checkHigh.id = bname + '_highBias';
            checkHigh.className = 'highBias';
            checkHigh.addEventListener('click', function(){updateBias(this);});

            var checkN = document.createElement('input');
            checkN.type = 'checkbox';
            checkN.id = bname + '_nBias';
            checkN.className = 'nBias';
            checkN.addEventListener('click', function(){updateBias(this);});
 
            var checkNLabel = document.createElement('label');
            checkNLabel.innerHTML = 'N';
            checkNLabel.htmlFor = bname + '_nBias';
 
            var checkHighLabel = document.createElement('label');
            checkHighLabel.innerHTML = 'H';
            checkHighLabel.htmlFor = bname + '_highBias';
 
            var strong = document.createElement('strong');
            strong.innerHTML = bname;

            item.appendChild(strong);
            item.id = bname;
            item.biasId = biasData.biasNames[biasGroup][bname];

            span.appendChild(coarseSel);
            span.appendChild(fineSel);
            span.appendChild(checkHigh);
            span.appendChild(checkHighLabel);
            span.appendChild(checkN);
            span.appendChild(checkNLabel);
            item.appendChild(span);
            list.appendChild(item);
        }
        bias_div.appendChild(list);
        bias_div_box.appendChild(bias_div);
        tabs.appendChild(bias_div_box);
    }
    bias_div_super.appendChild(tabs);
}


function get_json(jsonPath, processFct) {
	var request = new XMLHttpRequest();
	request.open('GET', jsonPath, true);
	
	request.onload = function() {
	  if (request.status >= 200 && request.status < 400){
	    // Success!
	    var jsonData = JSON.parse(request.responseText);
	    processFct(jsonData);
	  } else {
	    // We reached our target server, but it returned an error
	    alert('Could not access ' + jsonPath + ' on server (response: ' + request.status + ')');
	  }
	};
	
	request.onerror = function() {
	  // There was a connection error of some sort
	  alert('Could not connect to server to download ' + jsonPath + '.');
	};
	
	request.send();
}

//request experiment file
function loadBiases(el) {
    get_json('_biases_' + el.innerHTML + '.json', updateParams)
}

//request deletion of experiment file
function delBiases(el) {
    alert('NotImplemented');
}

//display list of files
function render_bias_list(expList) {
    var div = document.getElementById("biases_load");
    var div_div = div.getElementsByTagName('div')[0];
    var list = document.createElement('ul');
    list.className = 'filelist';
    for (var i = 0; i < expList.files.length; i++) {
	var item = document.createElement('li');
	var a1 = document.createElement('a');
	var a2 = document.createElement('a');
	var span = document.createElement('span');
        a1.addEventListener("click", function(){ loadBiases(this); });
        a2.addEventListener("click", function(){ delBiases(this); });
	a1.innerHTML = expList.files[i].substring(0, expList.files[i].length-5);
	a2.innerHTML = 'X';
	a1.href = '#';
	a2.href = '#';
	span.appendChild(a2);
	item.appendChild(a1);
	item.appendChild(span);
	list.appendChild(item);
    }
    div_div.appendChild(list);
}

//request bias data from server & trigger bias control init
get_json('_biaslist.json', function(biasData) {
    init_bias_ctl(biasData);
    get_json('_biases.json', render_bias_list);
    init_tabbing();
});

//extract bias data from element
function getBiasData(item) {
	var span = item.childNodes[1];
	var biasData = {};
	biasData.biasName = item.id;
	biasData.biasId = item.biasId;
	var params = span.childNodes;
	for (var i = 0; i < params.length; i++) {
		var paramName = params[i].className;
		var paramValue = params[i].value;
		if (paramName != undefined && paramValue != undefined) {
			if (paramName == 'highBias' || paramName == 'nBias')
				//convert to int
				paramValue = (params[i].checked ? 1 : 0);
			biasData[paramName] = parseInt(paramValue);
		}
	}
	return biasData;
}

function jsonRPCStr(method, params) {
	RPCObj = {};
	RPCObj.method = (method == undefined) ? "none" : method;
	RPCObj.params = (params == undefined) ? {} : params;
	return JSON.stringify(RPCObj);
}

//set biases
function updateBias(el) {
	var item = el.parentNode.parentNode;
	var RPCStr = jsonRPCStr("setBias", getBiasData(item));
	socket_di.send(RPCStr);
}

//update parameters according to experiment file
function updateParams(exp) {
    var biasData = exp.biasData;
    for (var i = 0; i < biasData.length; i++) {
	var item = document.getElementById(biasData[i].biasName);
	var params = item.getElementsByTagName('span')[0].childNodes;
	for (var j = 0; j < params.length; j++) {
	    var paramName = params[j].className;
	    var paramValue = params[j].value;
	    if (paramName != undefined && paramValue != undefined) {
		if (paramName == 'highBias' || paramName == 'nBias')
		    params[j].checked = (biasData[i][paramName] == 1 ? true : false);
		params[j].value = biasData[i][paramName];
	    }
	}
	updateBias(params[j-1]); //simply pass last element (parent will be used)
    }
}

//save all biases
function saveExperiment() {
	var nodeList = document.querySelectorAll(".biaslist");
	var params = {};
	var biases = [];
	for (var i = 0; i < nodeList.length; i++) {
		var items = nodeList[i].childNodes;
		for (var j = 0; j < items.length; j++) {
			var biasData = getBiasData(items[j]);
			biases.push(biasData);
		}
	}

	var fileName = prompt("Please enter a file name", "Experiment "+Date());
	if (fileName) {
		params.fileName = fileName + ".json";
		params.description = "no description";
		params.biasData = biases;
		var RPCStr = jsonRPCStr("saveBiases", params);
		alert(RPCStr);
		socket_di.send(RPCStr);
	}
}

