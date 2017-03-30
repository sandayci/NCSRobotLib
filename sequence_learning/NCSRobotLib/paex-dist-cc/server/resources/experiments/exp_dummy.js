
function Experiment () {
    this.title = "Dummy Experiment";
    this.description = "Use this template to build your own experiments";
    Exp = this;

    //init some status variables (mandatory)
    this.ready = false;
    this.getNext = false;
    this.getAuto = false;

    //create a canvas to draw on
    this.dummyCanvas = document.createElement('canvas');
    this.dummyCanvas.id = "canvas_dummy";
    this.dummyCanvas.width = 250;
    this.dummyCanvas.height = 250;
    this.canvasCtx = this.dummyCanvas.getContext('2d');

    //the update method is mandatory
    //it will be called whenever a new package of events arrives
    this.update = function(events) {
        //process events here...
    }

    //set some connections
    this.connect = function() {
        //define two groups of neurons
        this.grp_exc = [];
        this.grp_inh = [];
        for (var i=0; i<256; i++) {
           if (Math.random() < 0.25) { //25% inhibitory neurons
               this.grp_inh.push(i);
               rasterSetStyle(i, '#C00'); //display them as red
           }
           else {
               this.grp_exc.push(i);
               rasterSetStyle(i, '#0C0');
           }
        }
	//connect cells randomly
        for (var i=0; i < this.grp_exc.length; i++) {
            for (var j=0; j<256; j++) {
                if (Math.random() < 0.1) { //10% conn. probability
                    connectCells(this.grp_exc[i], j, 'exc', 1);
		    drawPixel(this.canvasCtx, {x:this.grp_exc[i], y:j}, 1);
	        }
            }
        }
        for (var i=0; i < this.grp_inh.length; i++) {
            for (var j=0; j<256; j++) {
                if (Math.random() < 0.1) { //10% conn. probability
                    connectCells(this.grp_inh[i], j, 'inh', 1);
		    drawPixel(this.canvasCtx, {x:this.grp_inh[i], y:j}, -1);
	        }
            }
        }
    }

    //add some control buttons
    expAddButton('Generate Network', Exp, 'connect', singleuse=true);

    //use expAddElement to add elements to experiment interface
    expAddElement(document.createElement('br'));
    expAddElement(this.dummyCanvas);

    //now we're ready to receive events
    this.ready = true;
}
