///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
////                                                  /////
////  Demo Sa&Ts dla TSP                              /////
////                                                  /////
////  M.Makuchowski                                   /////
////                                                  /////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

var nodeN = 32;
var node=[];              // dane instancji

var saPath=[];            // sa: ścieżka
var saDataCmax=[];        // sa: dane wykresu Cmax
var saBestCmax;           // sa: najlepsze znalezione Cmax
var saBestPath;           // sa: najlepsze znaleziona ścieżka
var saTemp;               // sa: temperatura

var tsPath=[];            // ts: ścieżka
var tsDataCmax=[];        // ts: dane wykresu Cmax
var tsBestCmax;           // ts: najlepsze znalezione Cmax
var tsBestPath;           // ts: najlepsze znaleziona ścieżka
var tsListaTabu=[];       // ts: listaTabu

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////                                                    ////
////  Główna pętla programu                             ////
////                                                    ////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

function Run()
{
  var startPath=RandPath();
  var startCmax=PathCmax(startPath);
  saPath=Array.from(startPath);
  tsPath=Array.from(startPath);
  saDataCmax=[]; saDataCmax.push(startCmax);
  tsDataCmax=[]; tsDataCmax.push(startCmax);
  saBestCmax=startCmax; saBestPath=Array.from(startPath);
  tsBestCmax=startCmax; tsBestPath=Array.from(startPath);

  tsListaTabu=[];
  isRun=1;
  loopN=340;
  saTemp=1000;
  Loop();
}

var loopN=0;
function Loop()
{
  if(loopN==0)return;
  loopN--;
  SaStep( saPath,saTemp ) ;
  TsStep( tsPath ) ;
  saTemp = saTemp*0.982;
  saDataCmax.push( PathCmax( saPath ));
  tsDataCmax.push( PathCmax( tsPath ));
  if(loopN==0)
  {
    saPath=saBestPath;
    tsPath=tsBestPath;
    saDataCmax.push(saBestCmax);
    tsDataCmax.push(tsBestCmax);
  }
  Draw();
  setTimeout(Loop,10);
}

//////////////////////////////////////////////////////////////
//                         SA                               //
//////////////////////////////////////////////////////////////
function SaStep(path,temp)
{
  var c0 = PathCmax(path);
  var praw=0,akce=0;
  var N= (node.length-1)*(node.length-2)/2;
  for(var i=0; i<N; i++)
  {
    var move = RandAB();
    PathMove( path,move );
    var c1 = PathCmax(path);
    if(c1>c0)
    { praw = Math.exp( (c0-c1)/temp );
      akce = ( Math.random()<praw );
    }
    if(c1<=c0||akce)  c0=c1; //akceptuje ruch
    else     PathMove( path,move ); //odrzucenie ruchu
    if(saBestCmax>c0)
    {
      saBestCmax=c0;
      saBestPath=Array.from( path );
    }
  }
}

//////////////////////////////////////////////////////////////
//                         TS                               //
//////////////////////////////////////////////////////////////

function TsStep(path)
{
  var move = TsFindBestMove( path );
  TsAddMoveTabu( path,move );
  PathMove( path,move );
}

function TsFindBestMove(path)
{
  var bestA=0,bestB=0,bestCmax=999999999;
  for(a=1;a<node.length-1;a++)
    for(b=a+1;b<node.length;b++)
    {
      PathMove(path,[a,b]);
      var cmax=PathCmax(path);

      if(TsIsPathTabu(path)==0 && cmax<bestCmax )
        {bestA=a;bestB=b;bestCmax=cmax;}

      if(cmax<tsBestCmax)
        {bestA=a;bestB=b;bestCmax=cmax;}

      if(tsBestCmax>bestCmax)
        { tsBestCmax=bestCmax; tsBestPath=Array.from( path ); }

      PathMove(path,[a,b]);
    }
  return [bestA,bestB];
}

function TsIsPathTabu(path)
{
  for(i=0; i<tsListaTabu.length; i++)
    if( path[tsListaTabu[i][0]]==tsListaTabu[i][1] ) return 1;
  return 0;
}

function TsAddMoveTabu( path,move )
{
  var a=move[0];
  var b=move[1];
  if((a==0)&&(b==0)) {tsListaTabu.splice(0,2); return;}
  tsListaTabu.push( [a,path[a]] );
  tsListaTabu.push( [b,path[b]] );
  if(tsListaTabu.length>64) tsListaTabu.splice(0,2);
}

//////////////////////////////////////////////////////////////
// inne                                                     //
//////////////////////////////////////////////////////////////
function NodeRand()
{
  isRun=0;
  node=[]; saPath=[]; tsPath=[];
  saDataCmax=[]; tsDataCmax=[];
  for(i=0;i<nodeN;i++)
    node.push([ Math.floor(Math.random()*340), Math.floor(Math.random()*200)]);
  Draw();
}

function RandAB()
{
  //Losowa paraliczb (od 1)
  var a=1+Math.floor(Math.random()*(node.length-1));
  var b=1+Math.floor(Math.random()*(node.length-2)); if(b>=a)b++;
  if(a>b) [a,b]=[b,a];
  return [a,b];
}

function RandPath()
{
  //losowa permutacja zaczynająca się od zera
  var path=[];
  for( i=0; i<node.length; i++) path.push(i);
  for (let i = path.length - 1; i > 0; i--)
  { const j = Math.floor(Math.random()*i+1 );
    [path[i], path[j]] = [path[j], path[i]];
  }
  return path;
}

function PathMove(path,move)
{
  var a=move[0], b=move[1];
  while(a<b){[path[a],path[b]]=[path[b],path[a]]; a++;b--;}
  //[path[a],path[b]]=[path[b],path[a]];
}

function PathCmax(path)
{
  var dist=0;
  for(var i=0; i<path.length; i++)
  { var n0 = path[i];
    var n1 = path[(i+1)%path.length];
    var x0 = node[n0][0];
    var y0 = node[n0][1];
    var x1 = node[n1][0];
    var y1 = node[n1][1];
    dist += Math.round(Math.sqrt((x0-x1)*(x0-x1)+(y0-y1)*(y0-y1)));
  }
  return dist;
}

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
////                                                  /////
////  Grafika                                         /////
////                                                  /////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

function NodeXY(id)
{
  var scale=   1;
  var shiftX=  30;
  var shiftY=  30;
  return [ shiftX+scale*node[id][0], shiftY+scale*node[id][1] ];
}

function SvgKrata()
{
  svg="";
  svg+='<g style="stroke: #303030; stroke-width:1; fill: none;">';
  svg+='<path d="M 30 230';
  for(var i=0;i<11;i++)svg+='m -10 0, l 360 0, m -350 -20';
  svg+='M 30 230';
  for(var i=0;i<18;i++)svg+='m 0 10, l 0 -220, m 20 210';
  svg+='"/></g>';
  return svg;
}

function SvgPath(path)
{
  var svg = '<svg width="400" height="260">';
  svg+=SvgKrata();
  if(path.length>0)
  { //Path
    svg+='<g style="stroke: #c0c070; stroke-width:2; fill: none;">';
    var x,y; [x,y] = NodeXY(path[0]);
    svg+='<path d="M '+x+' '+y;
    for(var i=1; i<path.length; i++ )
    { [x,y]= NodeXY(path[i]);
      svg+=', L '+x+' '+y;
    } svg+=',Z"/></g>';
  }
  //Node
  for(var i=0; i<node.length; i++ )
  { [x,y]= NodeXY(i);
    svg+='<circle cx="'+x+'" cy="'+y
       +'" r="3" stroke=#447744 stroke-width="1" fill=#447744 />'
  }
  svg+='</svg>';
  return svg;
}

function SvgDataCmax(data)
{
  var svg= '<svg width="400" height="300">';
  var x0=20,y0=260, x1=380,y2=260;
  svg+=SvgKrata();
  svg+='<line x1="20" y1="230" x2="380" y2="230" style="stroke:#707070;stroke-width:1" />';
  svg+='<line x1="30" y1="240" x2="30"  y2="20" style="stroke:#707070;stroke-width:1" />';

  if(data.length>0){
    var scala = 160/data[0];
    svg+='<g style="stroke: #c0c070; stroke-width:2; fill: none;">';
    svg+='<path d="M 30 70';
    for(var i=1;i<data.length;i++)
      svg+=',L '+(30+i)+' '+(230-scala*data[i]);
    svg+='"/></g>';
    svg+='<text x="320" y="45" fill="#707070" font-size="12" >'+
          +(data[data.length-1])+'</text>';
  }
  svg+='</svg>';
  return svg;
}

function Draw()
{
  document.getElementById("saPathSvg").innerHTML=SvgPath(saPath);
  document.getElementById("saCmaxSvg").innerHTML=SvgDataCmax(saDataCmax);
  document.getElementById("tsPathSvg").innerHTML=SvgPath(tsPath);
  document.getElementById("tsCmaxSvg").innerHTML=SvgDataCmax(tsDataCmax);
}

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
////                                                  /////
////  Start strony                                    /////
////                                                  /////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

function Init()
{
  document.onselectstart = function(){return false;};
  document.getElementById('saPath').innerHTML= '<div id="saPathTxt">SA: ścieżka</div> <div id="saPathSvg">path:svg</div>';
  document.getElementById('saCmax').innerHTML= '<div id="saCmaxTxt">SA: długość</div> <div id="saCmaxSvg">cmax:svg</div>';
  document.getElementById('tsPath').innerHTML= '<div id="tsPathTxt">TS: ścieżka</div> <div id="tsPathSvg">path:svg</div>';
  document.getElementById('tsCmax').innerHTML= '<div id="tsCmaxTxt">TS: długość</div> <div id="tsCmaxSvg">cmax:svg</div>';
  NodeRand();
}

Init();
