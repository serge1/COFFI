

//Tag Version: MSDN 1.6f/2-15-07
var gTagVer = "MSDN 1.6f/2-15-07";
var gService = true;
var gTimeZone = -8;
function dcsCookie(){
	if (typeof(dcsOther)=="function"){
		dcsOther();
	}
	else if (typeof(dcsPlugin)=="function"){
		dcsPlugin();
	}
	else if (typeof(dcsFPC)=="function"){
		dcsFPC(gTimeZone);
	}
}
function dcsGetCookie(name){
	var pos=document.cookie.indexOf(name+"=");
	if (pos!=-1){
		var start=pos+name.length+1;
		var end=document.cookie.indexOf(";",start);
		if (end==-1){
			end=document.cookie.length;
		}
		return unescape(document.cookie.substring(start,end));
	}
	return null;
}
function dcsGetCrumb(name,crumb){
	var aCookie=dcsGetCookie(name).split(":");
	for (var i=0;i<aCookie.length;i++){
		var aCrumb=aCookie[i].split("=");
		if (crumb==aCrumb[0]){
			return aCrumb[1];
		}
	}
	return null;
}
function dcsGetIdCrumb(name,crumb){
	var cookie=dcsGetCookie(name);
	var id=cookie.substring(0,cookie.indexOf(":lv="));
	var aCrumb=id.split("=");
	for (var i=0;i<aCrumb.length;i++){
		if (crumb==aCrumb[0]){
			return aCrumb[1];
		}
	}
	return null;
}
function dcsFPC(offset){
	if (typeof(offset)=="undefined"){
		return;
	}
	if (document.cookie.indexOf("WTLOPTOUT=")!=-1){
		return;
	}
	var name=gFpc;
	var dCur=new Date();
	var adj=(dCur.getTimezoneOffset()*60000)+(offset*3600000);
	dCur.setTime(dCur.getTime()+adj);
	var dExp=new Date(dCur.getTime()+315360000000);
	var dSes=new Date(dCur.getTime());
	if (document.cookie.indexOf(name+"=")==-1){
		if ((typeof(gWtId)!="undefined")&&(gWtId!="")){
			WT.co_f=gWtId;
		}
		else if ((typeof(gTempWtId)!="undefined")&&(gTempWtId!="")){
			WT.co_f=gTempWtId;
			WT.vt_f="1";
		}
		else{
			WT.co_f="2";
			var cur=dCur.getTime().toString();
			for (var i=2;i<=(32-cur.length);i++){
				WT.co_f+=Math.floor(Math.random()*16.0).toString(16);
			}
			WT.co_f+=cur;
			WT.vt_f="1";
		}
		if (typeof(gWtAccountRollup)=="undefined"){
			WT.vt_f_a="1";
		}
		WT.vt_f_s="1";
		WT.vt_f_d="1";
		WT.vt_f_tlh=WT.vt_f_tlv="0";
	}
	else{
		var id=dcsGetIdCrumb(name,"id");
		var lv=parseInt(dcsGetCrumb(name,"lv"));
		var ss=parseInt(dcsGetCrumb(name,"ss"));
		if ((id==null)||(id=="null")||isNaN(lv)||isNaN(ss)){
			return;
		}
		WT.co_f=id;
		var dLst=new Date(lv);
		WT.vt_f_tlh=Math.floor((dLst.getTime()-adj)/1000);
		dSes.setTime(ss);
		if ((dCur.getTime()>(dLst.getTime()+1800000))||(dCur.getTime()>(dSes.getTime()+28800000))){
			WT.vt_f_tlv=Math.floor((dSes.getTime()-adj)/1000);
			dSes.setTime(dCur.getTime());
			WT.vt_f_s="1";
		}
		if ((dCur.getDay()!=dLst.getDay())||(dCur.getMonth()!=dLst.getMonth())||(dCur.getYear()!=dLst.getYear())){
			WT.vt_f_d="1";
		}
	}
	WT.co_f=escape(WT.co_f);
	WT.vt_sid=WT.co_f+"."+(dSes.getTime()-adj);
	var expiry="; expires="+dExp.toGMTString();
	document.cookie=name+"="+"id="+WT.co_f+":lv="+dCur.getTime().toString()+":ss="+dSes.getTime().toString()+expiry+"; path=/"+(((typeof(gFpcDom)!="undefined")&&(gFpcDom!=""))?("; domain="+gFpcDom):(""));
	if (document.cookie.indexOf(name+"=")==-1){
		WT.co_f=WT.vt_sid=WT.vt_f_s=WT.vt_f_d=WT.vt_f_tlh=WT.vt_f_tlv="";
		WT.vt_f=WT.vt_f_a="2";
	}
}

var gFpcDom=".microsoft.com";

function dcsEvt(evt,tag){
	var e=evt.target||evt.srcElement;
	while (e.tagName&&(e.tagName!=tag)){
		e=e.parentElement||e.parentNode;
	}
	return e;
}
function dcsBind(event,func){
	if ((typeof(window[func])=="function")&&document.body){
		if (document.body.addEventListener){
			document.body.addEventListener(event, window[func], true);
		}
		else if(document.body.attachEvent){
			document.body.attachEvent("on"+event, window[func]);
		}
	}
}
function dcsET(){
	dcsBind("click","dcsDownload");
	dcsBind("click","dcsFormButton");
	dcsBind("keypress","dcsFormButton");
	dcsBind("click","dcsImageMap");
}
function dcsMultiTrack(){
	if (arguments.length%2==0){
		for (var i=0;i<arguments.length;i+=2){
			if (arguments[i].indexOf('WT.')==0){
				WT[arguments[i].substring(3)]=arguments[i+1];
			}
			else if (arguments[i].indexOf('DCS.')==0){
				DCS[arguments[i].substring(4)]=arguments[i+1];
			}
			else if (arguments[i].indexOf('DCSext.')==0){
				DCSext[arguments[i].substring(7)]=arguments[i+1];
			}
		}
		var dCurrent=new Date();
		DCS.dcsdat=dCurrent.getTime();
		dcsTag();
	}
}
function dcsDownload(evt){
	evt=evt||(window.event||"");
	if (evt){
		var e=dcsEvt(evt,"A");
		if(e){
			if (e.hostname&&e.href&&e.protocol&&(e.protocol.indexOf("http")!=-1)){
				dcsNavigation(e);
				var gPath=e.pathname?((e.pathname.indexOf("/")!=0)?"/"+e.pathname:e.pathname):"/";				
				if(document.all){var gTitle=e.innerText||e.innerHTML||"";}else{var gTitle=e.text||e.innerHTML||"";}
				dcsMultiTrack("DCS.dcssip",e.hostname,"DCS.dcsuri",gPath,"DCS.dcsqry",e.search||"","WT.ti","Link:"+gTitle,"WT.dl","1","WT.ad","","WT.mc_id","","WT.sp","");
				DCS.dcssip=DCS.dcsuri=DCS.dcsqry=WT.ti=WT.dl=gTitle=gPath="";
			}
		}
	}
}
function dcsNavigation(wtnode){
	try{
	var wtCount=0;
	while(wtCount!=1){
		if(wtnode.parentNode.tagName!="DIV"){
			wtnode=wtnode.parentNode;
		}
		if(wtnode.parentNode.tagName=="DIV"){
			if((wtnode.parentNode.getAttribute('id'))||(wtnode.parentNode.className)){
				DCSext.wtNavigation = wtnode.parentNode.getAttribute("id")||wtnode.parentNode.className;
				wtCount=1;
			}
			else{
				wtnode=wtnode.parentNode;
			}		
		}
	}}
	catch(error){}				
}
function dcsImageMap(evt){
	evt=evt||(window.event||"");
	if (evt){
		var f=dcsEvt(evt,"AREA");
		if(f){
			if (f.hostname&&f.href&&f.protocol&&(f.protocol.indexOf("http")!=-1)){
				var path=f.pathname?((f.pathname.indexOf("/")!=0)?"/"+f.pathname:f.pathname):"/";
				dcsMultiTrack("DCS.dcssip",f.hostname,"DCS.dcsuri",path,"DCS.dcsqry",f.search||"","WT.ti","Link:Image Map","WT.dl","1","WT.ad","","WT.mc_id","","WT.sp","");
				DCS.dcssip=DCS.dcsuri=DCS.dcsqry=WT.ti=WT.dl="";
			}			
		}
	}
}
function dcsFormButton(evt){
	evt=evt||(window.event||"");
	if (evt){
		var e=dcsEvt(evt,"INPUT");
		var type=e.type||"";
		if (type&&((type=="submit")||(type=="image")||(type=="button")||(type=="reset"))||((type=="text")&&((evt.which||evt.keyCode)==13))){
			var gUri=gTitle=gMethod=qry="";
			if(e.form){
				var elems=e.form.elements;
				for (var i=0;i<elems.length;i++){
					var etype=elems[i].type;
					if (etype=="text"){
						qry+=((qry=="")?"":"&")+escape(elems[i].name)+"="+escape(elems[i].value);
					}
				}
				gUri=e.form.action||window.location.pathname;
				gTitle=e.form.id||e.form.className||e.form.name||"Unknown";
				gMethod=e.form.method||"Unknown";
			}
			else{
				gUri=window.location.pathname;
				gTitle=e.name||e.id||"Unknown";
				gMethod="Input";
			}
			if((gUri!="")&&(gTitle!="")&&(gMethod!="")&&(evt.keyCode!=9)){
				dcsMultiTrack("DCS.dcsuri",gUri,"DCS.dcsqry",qry,"WT.ti","FormButton:"+gTitle,"WT.dl","2","WT.fm",gMethod,"WT.ad","","WT.mc_id","","WT.sp","");
			}
			DCS.dcsuri=DCS.dcsqry=qry=WT.ti=WT.dl=WT.fm=gUri=gTitle=gMethod="";
		}
	}
}
function dcsAdSearch()
{
}
function dcsContentRatings(evt){
	evt=evt||(window.event||"");
	if (evt){
		var e=dcsEvt(evt,"A");
		if(e){
			if (e.hostname){
				if(document.all){var gTitle=DCSext.wt_ratd2=e.innerText||e.innerHTML||"";}else{var gTitle=DCSext.wt_ratd2=e.text||e.innerHTML||"";}
				DCSext.wt_ratd1="Upper Ratings Options";
				dcsMultiTrack("DCS.dcssip",frmRatings.window.location.hostname,"DCS.dcsuri",frmRatings.window.location.pathname,"DCS.dcsqry","","WT.ti","Link:"+gTitle,"WT.dl","9","WT.ad","","WT.mc_id","","WT.sp","");
				DCS.dcssip=DCS.dcsuri=DCS.dcsqry=WT.ti=WT.dl=gTitle=DCSext.wt_ratd1=DCSext.wt_ratd2="";
			}
		}
		var f=dcsEvt(evt,"INPUT");
		if(f){
			var type=f.type||"";
			if (type&&((type=="submit")||(type=="button")&&((evt.which||evt.keyCode)==13))){
				var gUri=gTitle="";
				gUri=f.form.action||frmRatings.window.location.pathname;
				gTitle="Content Ratings";
				var gComTemp=frmRatings.document.getElementById("txtFeedback");
				if (gComTemp.value!=""){DCSext.wt_ratd2="Comment Submitted";}
				else{DCSext.wt_ratd2="No Comment Submitted";}			
				try{var gCount=1;
 				while(frmRatings.document.getElementById("Rate"+gCount)!="undefined"){
 					var gRatTemp = frmRatings.document.getElementById("Rate"+gCount);
 					if (gRatTemp.checked){DCSext.wt_ratings=gRatTemp.value.replace(/rate/gi,"");}
 					gCount++;
 				}}
 				catch(error){}
				DCSext.wt_ratd1="Comment Fields";			
				if((gUri!="")&&(evt.keyCode!=9)){
					dcsMultiTrack("DCS.dcsuri",gUri,"DCS.dcsqry","","WT.ti","FormButton:"+gTitle,"WT.dl","cr","WT.ad","","WT.mc_id","","WT.sp","");
				}
				DCS.dcsuri=DCS.dcsqry=qry=WT.ti=WT.dl=WT.fm=DCSext.wt_ratings=DCSext.wt_ratd1=DCSext.wt_ratd2=gTitle=gUri="";
			}
		}
	}	
}
function dcsAdv(){
	if ((typeof(gTrackEvents)!="undefined")&&gTrackEvents){
		WT.wtsv=1;
		if(typeof(WT.sp)!="undefined"){WT.sv_sp=WT.sp;}
		dcsFunc("dcsET");
	}
	dcsFunc("dcsCookie");
	//dcsFunc("dcsAdSearch");
	dcsFunc("dcsNvrRu");
	if(document.getElementById('frmRatings')){
		var gTemp = document.getElementById('frmRatings').src;
		if(gTemp.indexOf("ratings.aspx")!=-1){
			if (frmRatings.document.body.addEventListener){
					frmRatings.document.body.addEventListener("click",dcsContentRatings, true);
					frmRatings.document.body.addEventListener("keypress",dcsContentRatings, true);
			}	
			else if(frmRatings.document.body.attachEvent){
					frmRatings.document.body.attachEvent("onclick",dcsContentRatings);
					frmRatings.document.body.attachEvent("keypress",dcsContentRatings);
			}
		}
	}
}

var gImages=new Array;
var gIndex=0;
var DCS=new Object();
var WT=new Object();
var DCSext=new Object();
var gQP=new Array();
var gI18n=false;
if (window.RegExp){
	var RE={"%09":/\t/g,"%20":/ /g,"%23":/\#/g,"%26":/\&/g,"%2B":/\+/g,"%3F":/\?/g,"%5C":/\\/g};
	var I18NRE={"%25":/\%/g};
}

function dcsVar(){
	var dCurrent=new Date();
	WT.tz=dCurrent.getTimezoneOffset()/60*-1;
	if (WT.tz==0){
		WT.tz="0";
	}
	WT.bh=dCurrent.getHours();
	WT.ul=navigator.appName=="Netscape"?navigator.language:navigator.userLanguage;
	if (typeof(screen)=="object"){
		WT.cd=navigator.appName=="Netscape"?screen.pixelDepth:screen.colorDepth;
		WT.sr=screen.width+"x"+screen.height;
	}
	if (typeof(navigator.javaEnabled())=="boolean"){
		WT.jo=navigator.javaEnabled()?"Yes":"No";
	}
	if (document.title){
		WT.ti=gI18n?dcsEscape(dcsEncode(document.title),I18NRE):document.title;
	}
	WT.js="Yes";
	WT.jv=dcsJV();
	if (document.body&&document.body.addBehavior){
		document.body.addBehavior("#default#clientCaps");
		if (document.body.connectionType){
			WT.ct=document.body.connectionType;
		}
		document.body.addBehavior("#default#homePage");
		WT.hp=document.body.isHomePage(location.href)?"1":"0";
	}
	if (parseInt(navigator.appVersion)>3){
		if ((navigator.appName=="Microsoft Internet Explorer")&&document.body){
			WT.bs=document.body.offsetWidth+"x"+document.body.offsetHeight;
		}
		else if (navigator.appName=="Netscape"){
			WT.bs=window.innerWidth+"x"+window.innerHeight;
		}
	}
	WT.fi="No";
	if (window.ActiveXObject){
		for(var i=10;i>0;i--){
			try{
				var flash = new ActiveXObject("ShockwaveFlash.ShockwaveFlash."+i);
				WT.fi="Yes";
				WT.fv=i+".0";
				break;
			}
			catch(e){
			}
		}
	}
	else if (navigator.plugins&&navigator.plugins.length){
		for (var i=0;i<navigator.plugins.length;i++){
			if (navigator.plugins[i].name.indexOf('Shockwave Flash')!=-1){
				WT.fi="Yes";
				WT.fv=navigator.plugins[i].description.split(" ")[2];
				break;
			}
		}
	}
	if (gI18n){
		WT.em=(typeof(encodeURIComponent)=="function")?"uri":"esc";
		if (typeof(document.defaultCharset)=="string"){
			WT.le=document.defaultCharset;
		} 
		else if (typeof(document.characterSet)=="string"){
			WT.le=document.characterSet;
		}
	}
	if (typeof(wtsp)!="undefined"){WT.sp=wtsp;}	
	WT.dl="0";
	WT.dcsvid=dcsGetCookie("MC1");
	DCS.dcsdat=WT.dcsdat=dCurrent.getTime();
	DCS.dcssip=window.location.hostname;
	DCS.dcsuri=window.location.pathname;
	var currentTime = new Date();
	DCSext.wt_date = currentTime.getFullYear() + "/" + (currentTime.getMonth() + 1) + "/" + currentTime.getDate();
	DCSext.wt_dos=1;
	var gDirLevels = 5;
	var gFpath = window.location.pathname.substring(window.location.pathname.indexOf('/')+1,window.location.pathname.lastIndexOf('/')+1).toLowerCase();
	if(gFpath==''){gFpath="/";}	
	else{	
		var gSplit=gFpath.split("/");
		gFpath="";
		for(var i=1;i<gSplit.length&&i<=gDirLevels;i++){
			gFpath+="/";
			for(var b=0;b<i;b++){
					gFpath+=gSplit[b]+"/";
			}
			if(i!=gDirLevels&&i!=gSplit.length-1){
				gFpath+=";";
			}		
		}
	}
	DCSext.wtDrillDir=gFpath;
	DCSext.wtEvtSrc=window.location.hostname+window.location.pathname;
	if (window.location.search){
		DCS.dcsqry=window.location.search;
		if (gQP.length>0){
			for (var i=0;i<gQP.length;i++){
				var pos=DCS.dcsqry.indexOf(gQP[i]);
				if (pos!=-1){
					var front=DCS.dcsqry.substring(0,pos);
					var end=DCS.dcsqry.substring(pos+gQP[i].length,DCS.dcsqry.length);
					DCS.dcsqry=front+end;
				}
			}
		}
	}
	if ((window.document.referrer!="")&&(window.document.referrer!="-")){
		if (!(navigator.appName=="Microsoft Internet Explorer"&&parseInt(navigator.appVersion)<4)){
			DCS.dcsref=gI18n?dcsEscape(window.document.referrer, I18NRE):window.document.referrer;
		}
	}
}
function dcsA(N,V){
	return "&"+N+"="+dcsEscape(V, RE);
}
function dcsEscape(S, REL){
	if (typeof(REL)!="undefined"){
		var retStr = new String(S);
		for (R in REL){
			retStr = retStr.replace(REL[R],R);
		}
		return retStr;
	}
	else{
		return escape(S);
	}
}
function dcsEncode(S){
	return (typeof(encodeURIComponent)=="function")?encodeURIComponent(S):escape(S);
}
function dcsCreateImage(dcsSrc){
	if (document.images){
		gImages[gIndex]=new Image;
		gImages[gIndex].src=dcsSrc;
		gIndex++;
	}
	else{
		document.write('<IMG ALT="" BORDER="0" NAME="DCSIMG" WIDTH="1" HEIGHT="1" SRC="'+dcsSrc+'">');
	}
}
function dcsMeta(){
	var elems;
	if (document.all){
		elems=document.all.tags("meta");
	}
	else if (document.documentElement){
		elems=document.getElementsByTagName("meta");
	}
	if (typeof(elems)!="undefined"){
		for (var i=1;i<=elems.length;i++){
			var meta=elems.item(i-1);
			if (meta.name){
				if (meta.name.indexOf('WT.')==0){
					WT[meta.name.substring(3)]=(gI18n&&(meta.name.indexOf('WT.ti')==0))?dcsEscape(dcsEncode(meta.content),I18NRE):meta.content;
				}
				else if (meta.name.indexOf('DCSext.')==0){
					DCSext[meta.name.substring(7)]=meta.content;
				}
				else if (meta.name.indexOf('DCS.')==0){
					DCS[meta.name.substring(4)]=(gI18n&&(meta.name.indexOf('DCS.dcsref')==0))?dcsEscape(meta.content,I18NRE):meta.content;
				}
			}
		}
	}
}
function dcsTag(){
	if (document.cookie.indexOf("WTLOPTOUT=")!=-1){
		return;
	}
	var P="http"+(window.location.protocol.indexOf('https:')==0?'s':'')+"://"+gDomain+(gDcsId==""?'':'/'+gDcsId)+"/dcs.gif?";
	for (N in DCS){
		if (DCS[N]) {
			P+=dcsA(N,DCS[N]);
		}
	}
	for (N in WT){
		if (WT[N]) {
			P+=dcsA("WT."+N,WT[N]);
		}
	}
	for (N in DCSext){
		if (DCSext[N]) {
			P+=dcsA(N,DCSext[N]);
		}
	}
	if (P.length>2048&&navigator.userAgent.indexOf('MSIE')>=0){
		P=P.substring(0,2040)+"&WT.tu=1";
	}
	dcsCreateImage(P);
}
function dcsJV(){
	var agt=navigator.userAgent.toLowerCase();
	var major=parseInt(navigator.appVersion);
	var mac=(agt.indexOf("mac")!=-1);
	var nn=((agt.indexOf("mozilla")!=-1)&&(agt.indexOf("compatible")==-1));
	var nn4=(nn&&(major==4));
	var nn6up=(nn&&(major>=5));
	var ie=((agt.indexOf("msie")!=-1)&&(agt.indexOf("opera")==-1));
	var ie4=(ie&&(major==4)&&(agt.indexOf("msie 4")!=-1));
	var ie5up=(ie&&!ie4);
	var op=(agt.indexOf("opera")!=-1);
	var op5=(agt.indexOf("opera 5")!=-1||agt.indexOf("opera/5")!=-1);
	var op6=(agt.indexOf("opera 6")!=-1||agt.indexOf("opera/6")!=-1);
	var op7up=(op&&!op5&&!op6);
	var jv="1.1";
	if (nn6up||op7up){
		jv="1.5";
	}
	else if ((mac&&ie5up)||op6){
		jv="1.4";
	}
	else if (ie5up||nn4||op5){
		jv="1.3";
	}
	else if (ie4){
		jv="1.2";
	}
	return jv;
}
function dcsFunc(func){
	if (typeof(window[func])=="function"){
		window[func]();
	}
}

function dcsNvrRu() {
    if (document.cookie.indexOf("WTLOPTOUT=") != -1) {
        return;
    }
    var gc = function(name) {
        var c = document.cookie;
        var pos = c.indexOf(name + "=");
        var start, end;
        if (pos != -1) {
            start = pos + name.length + 1;
            end = c.indexOf(";", start);
            if (end == -1) {
                end = c.length;
            }
            return c.substring(start, end);
        }
        return null;
    };
    var cur = new Date();
    var exp = new Date(cur.getTime() + 315360000000);
    var nvrRu = new DcsNvrRu(gc, exp);
    var nvr = new DcsNvr(gc, exp);
    nvrRu.run();
    nvr.run();
}

function DcsNvrRu(gc, exp) {
    var cfg = {
        fpcname: wt_nvr_ru,
        fpcdom: wt_fpcdom,
        domlist: wt_domlist,
        pathlist: wt_pathlist,
        paramlist: wt_paramlist,
        siteid: wt_siteid
    };
    var data = [];

    this.run = function() {
        read();
        var rc = process();
        var c, i, val;
        if (rc) {
            c = compose();
            for (i = 0; i < c.length; i++) {
                document.cookie = c[i];
            }
            val = (document.cookie.indexOf(cfg.fpcname + "=") != -1) ? "1" : "2";
            if ((rc & 1) || (rc & 2)) {
                WT.vt_nvr0 = val;
            }
            if (rc & 4) {
                WT.vt_nvr1 = val;
            }
        }
    };

    function read() {
        var num = 0;
        var value = gc(cfg.fpcname);
        var levels, level, crumbs, curlevel, leveldata;
        while (value) {
            levels = value.split(":");
            for (level = 0; level < levels.length; level++) {
                crumbs = levels[level].split("=");
                curlevel = crumbs[0];
                leveldata = [];
                if (crumbs[1].length > 0) {
                    leveldata = crumbs[1].split("|");
                }
                if (typeof (data[curlevel]) != "object") {
                    data[curlevel] = leveldata;
                }
                else {
                    data[curlevel] = data[curlevel].concat(leveldata);
                }
            }
            num++;
            value = gc(cfg.fpcname + num);
        }
    }

    function process() {
        var rc = 0;
        var rudom = getRuDom();
        var rupath = getRuPath();
        var ruparam = getRuParam();
        if (data.length > 0) {
            if ((rudom.length > 0) && update(0, rudom)) {
                rc |= 1;
            }
            if ((rupath.length > 0) && update(1, rupath)) {
                rc |= 2;
            }
            if ((ruparam.length > 2) && updateParam(2, ruparam)) {
                rc |= 4;
            }
        }
        else {
            if (rudom.length > 0) {
                data[0] = [rudom];
                rc |= 1;
            }
            else {
                data[0] = [];
            }
            if (rupath.length > 0) {
                data[1] = [rupath];
                rc |= 2;
            }
            else {
                data[1] = [];
            }
            if (ruparam.length > 0) {
                data[2] = [ruparam];
                rc |= 4;
            }
            else {
                data[2] = [];
            }
        }
        return rc;
    }

    function compose() {
        var cookies = [];
        var attr = { name: cfg.fpcname, value: "", expiry: "; expires=" + exp.toGMTString(), path: "; path=/", domain: "; domain=" + cfg.fpcdom };
        var num = 0;
        var maxval = 4000;
        var newcrumb, j;
        for (var i = 0; i < data.length; i++) {
            newcrumb = i + "=";
            for (j = 0; j < data[i].length; j++) {
                newcrumb += ((j === 0) ? "" : "|") + data[i][j];
            }
            if (((attr.name + (num || "")).length + attr.value.length + newcrumb.length + 1) <= maxval) {
                if (attr.value.length > 0) {
                    attr.value += ":";
                }
                attr.value += newcrumb;
            }
            else {
                cookies[num] = attr.name + (num || "") + "=" + attr.value + attr.expiry + attr.path + attr.domain;
                attr.value = newcrumb;
                num++;
            }
        }
        cookies[num] = attr.name + (num || "") + "=" + attr.value + attr.expiry + attr.path + attr.domain;
        return cookies;
    }

    function getRuDom() {
        var curdom = getCurDom();
        var doms = getDoms();
        for (var i = 0; i < doms.length; i++) {
            if (doms[i] == curdom) {
                return doms[i];
            }
        }
        return "";
    }

    function getRuPath() {
        var curpath = getCurPath();
        var paths = getPaths();
        for (var i = 0; i < paths.length; i++) {
            if (curpath.indexOf(paths[i]) != -1) {
                return paths[i];
            }
        }
        return "";
    }

    function getRuParam() {
        var params = getParams();
        var value, i, parts;
        for (i = 0; i < params.length; i++) {
            value = eval(params[i]);
            if (value && (value.length > 0)) {
                return params[i] + "&" + value;
            }
        }
        var curparams = getCurParams();
        var pair, j;
        for (i = 0; i < curparams.length; i++) {
            pair = curparams[i].split("=");
            if (pair.length == 2) {
                for (j = 0; j < params.length; j++) {
                    if (params[j] == pair[0]) {
                        return pair[0] + "&" + pair[1];
                    }
                }
            }
        }
        return "";
    }

    function update(level, item) {
        var items = data[level];
        for (var i = 0; i < items.length; i++) {
            if (items[i] == item) {
                return false;
            }
        }
        data[level][data[level].length] = item;
        return true;
    }

    function updateParam(level, item) {
        var pair = item.split("&");
        var param, j;
        for (var i = 0; i < data[level].length; i++) {
            param = data[level][i].split("&");
            if (param[0] == pair[0]) {
                for (j = 1; j < param.length; j++) {
                    if (param[j] == pair[1]) {
                        return false;
                    }
                }
                data[level][i] += "&" + pair[1];
                return true;
            }
        }
        data[level][data[level].length] = pair[0] + "&" + pair[1];
        return true;
    }

    function clipDom(dom, fpcdom) {
        var idx = dom ? dom.indexOf(fpcdom) : -1;
        return (idx != -1) ? dom.substring(0, idx) : dom;
    }

    function getCurDom() {
        var dom = clipDom(window.location.hostname.toLowerCase(), cfg.fpcdom.toLowerCase());
        if (dom.length === 0) {
            dom = "www";
        }
        return dom;
    }

    function getDoms() {
        var doms = cfg.domlist.toLowerCase().split(",");
        for (var i = 0; i < doms.length; i++) {
            doms[i] = clipDom(doms[i], cfg.fpcdom.toLowerCase());
            if (doms[i].length === 0) {
                doms[i] = "www";
            }
        }
        return doms;
    }

    function getPaths() {
        var paths = cfg.pathlist.toLowerCase().split(",");
        var p;
        for (var i = 0; i < paths.length; i++) {
            p = paths[i];
            if (p.length === 0) {
                p = "/";
            }
            else if (p.length > 1) {
                if (p.charAt(0) == "/") {
                    p = p.substr(1);
                }
                if (p.charAt(p.length - 1) == "/") {
                    p = p.substr(0, p.length - 1);
                }
            }
            if (p.length != paths[i].length) {
                paths[i] = p;
            }
            if (cfg.siteid.length > 0) {
                paths[i] = cfg.siteid + "&" + paths[i];
            }
        }
        return paths;
    }

    function getCurPath() {
        var p = window.location.pathname;
        p = p.substring(p.indexOf("/") + 1, p.lastIndexOf("/")).toLowerCase();
        var badchars = { "%09": /\t/g, "%20": / /g, "%2C": /,/g, "%3B": /;/g };
        for (var ch in badchars) {
            p = p.replace(badchars[ch], ch);
        }
        return (cfg.siteid.length > 0) ? cfg.siteid + "&" + p : p;
    }

    function getCurParams() {
        return (typeof (DCS.dcsqry) != "undefined") ? DCS.dcsqry.substring(1).split("&") : [];
    }

    function getParams() {
        return cfg.paramlist.split(",");
    }
}

function DcsNvr(gc, exp) {
    var cfg = {
        fpcname: "WT_NVR",
        fpcdom: window.location.hostname,
        maxlevel: 3
    };
    var data = [];

    this.run = function() {
        read();
        var c, i, val;
        if (process()) {
            c = compose();
            for (i = 0; i < c.length; i++) {
                document.cookie = c[i];
            }
            val = (document.cookie.indexOf(cfg.fpcname + "=") != -1) ? "1" : "2";
            for (i = 0; i < data.length; i++) {
                if (data[i][1]) {
                    WT["vt_nvr" + (i + 2)] = val;
                }
            }
        }
    };

    function read() {
        var num = 0;
        var re = /,/g;
        var value = gc(cfg.fpcname);
        var levels, i, crumbs, level, paths;
        while (value) {
            levels = value.split(":");
            for (i = 0; i < levels.length; i++) {
                crumbs = levels[i].split("=");
                level = parseInt(crumbs[0], 10);
                paths = crumbs[1].replace(re, "|").split("|");
                if (typeof (data[level]) != "object") {
                    data[level] = [paths, false];
                }
                else {
                    data[level][0] = data[level][0].concat(paths);
                }
            }
            num++;
            value = gc(cfg.fpcname + num);
        }
    }

    function process() {
        var newv = false;
        var curlevel = 0;
        var splitp = [];
        var p = window.location.pathname;
        var curpath = p.substring(p.indexOf("/") + 1, p.lastIndexOf("/")).toLowerCase();
        var badchars = { "%09": /\t/g, "%20": / /g, "%2C": /,/g, "%3B": /;/g };
        for (var ch in badchars) {
            curpath = curpath.replace(badchars[ch], ch);
        }

        if (curpath.length > 1) {
            splitp = curpath.split("/", cfg.maxlevel);
            curlevel = splitp.length;
            curpath = splitp.join("/");
        }
        var found = false;
        var i;
        if (data.length > 0) {
            if (data.length > curlevel) {
                for (i = 0; i < data[curlevel][0].length; i++) {
                    if (data[curlevel][0][i] == curpath) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    data[curlevel][0][data[curlevel][0].length] = curpath;
                    data[curlevel][1] = true;
                    newv = true;
                }
            }
            else {
                for (i = 0; i < (curlevel + 1); i++) {
                    if (typeof (data[i]) != "object") {
                        data[i] = [[(i === 0) ? "/" : splitp.slice(0, i).join("/")], true];
                    }
                }
                newv = true;
            }
        }
        else {
            for (i = 0; i < (curlevel + 1); i++) {
                data[i] = [[(i === 0) ? "/" : splitp.slice(0, i).join("/")], true];
            }
            newv = true;
        }
        return newv;
    }

    function compose() {
        var cookies = [];
        var attr = { name: cfg.fpcname, value: "", expiry: "; expires=" + exp.toGMTString(), path: "; path=/", domain: "; domain=" + cfg.fpcdom };
        var paths = [];
        var num = 0;
        var maxnum = 10;
        var maxval = 4000;
        var maxed = false;
        var newpath, j;
        for (var i = 0; (i < data.length) && !maxed; i++) {
            paths = data[i][0];
            newpath = i + "=";
            for (j = 0; (j < paths.length) && !maxed; j++) {
                newpath += ((j === 0) ? "" : "|") + paths[j];
                if (((attr.name + num).length + attr.value.length + newpath.length + 1) <= maxval) {
                    if ((attr.value.length > 0) && (j === 0)) {
                        attr.value += ":";
                    }
                    attr.value += newpath;
                }
                else if (attr.value.length > 0) {
                    cookies[num] = attr.name + (num || "") + "=" + attr.value + attr.expiry + attr.path + attr.domain;
                    attr.value = i + "=" + paths[j];
                    if ((++num) > (maxnum - 1)) {
                        maxed = true;
                    }
                }
                newpath = "";
            }
        }
        if (!maxed) {
            cookies[num] = attr.name + (num || "") + "=" + attr.value + attr.expiry + attr.path + attr.domain;
        }
        return cookies;
    }
}


dcsVar();
dcsMeta();
dcsFunc("dcsAdv");
dcsTag();


