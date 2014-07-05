

var CCE_EditorPrefix;
var CCE_EditorContainerPrefix;
var CCE_Editor = null;
var CCE_TitleID;
var CCE_ButtonsID;
var CCE_LicenseID;
var CCE_SubmitBtnID;
var CCE_SubmitText;
var CCE_CancelBtnID;
var CCE_MessageLabelID;
var CCE_CurrentAnnotation = -1;
var CCE_AnnotationToDelete = -1;
var CCE_OriginalTitle = "";
var CCE_OriginalContent = "";
var CCE_OriginalTags = "";
var CCE_RemoveMouseEffectFunc = null;
var CCE_IsIE = document.all;

function CCE_CancelAnnotation() {
    // Hide the editor
    CCE_HideEditor(CCE_CurrentAnnotation, CCE_GetControlPrefixes(CCE_CurrentAnnotation).control);
}

function CCE_CheckCookies() {
    var cookieParts = document.cookie.split("; ");
    var crumb = "";
    for (var i = 0; i < cookieParts.length; i++) {
        var crumbs = cookieParts[i].split("=");
        if (crumbs[0] == "wbi"
		 && crumbs[1] != "") {
            // Remove cookie
            document.cookie = "wbi=; expires=Fri, 02-Jan-1970 00:00:00 GMT";

            var crumbParts = crumbs[1].split(":");
            if (crumbParts[0] == "edit"
		   && crumbParts[1] != "") {
                CCE_EditAnnotation(crumbParts[1]);
            }
        }
    }
}

// idx is annotation index
function CCE_DeleteAnnotation(idx) {
    if (idx < 0) {
        return; // Invalid annotation index
    }

    CCE_InitEditor();

    // Verify that the moderator really wants to delete
    if (confirm(CCI_DeleteConfirmationText)) {
        CCE_AnnotationToDelete = idx;

        // Disable the Delete button
        var deleteBtn = $get(CCE_GetControlPrefixes(CCE_AnnotationToDelete).container + "Delete");
        deleteBtn.disabled = true;

        // Delete the annotation
        Microsoft.Mtps.Web.WebServices.CCWebService.DeleteAnnotation(CCS_User,
                                                                 CCS_ContentUrl,
                                                                 CCE_AnnotationToDelete,
                                                                 CCE_DeleteAnnotationComplete,
                                                                 CCE_DeleteAnnotationFailure);
    }
}

function CCE_DeleteAnnotationComplete(result) {
    // Remove the annotation
    var annotationElem = $get(CCE_GetControlPrefixes(CCE_AnnotationToDelete).container + "Container");
    annotationElem.style.display = "none";

    CCE_AnnotationToDelete = -1;
}

function CCE_DeleteAnnotationFailure(error) {
    // Show the error
    var deleteBtn = $get(CCE_GetControlPrefixes(CCE_AnnotationToDelete).container + "Delete");
    deleteBtn.disabled = false;
    CCE_ShowError(deleteBtn, error.get_message());

    CCE_AnnotationToDelete = -1;
}

// idx is annotation index (-1 is a new annotation)
function CCE_EditAnnotation(idx) {
    if (CCE_IsEditing) {
        return;
    }

    CCE_InitEditor();

    CCE_Editor.tooltip.hide();
    CCE_SubmitText = $get(CCE_SubmitBtnID).value;
    CCE_CurrentAnnotation = idx;

    if (idx != -1) { // User is editing an existing annotation
        var prefixes = CCE_GetControlPrefixes(idx);
        var annotationPrefix = prefixes.control;
        var annotationContainerPrefix = prefixes.container;
        var annotationElem = $get(annotationPrefix + "RadEditor");
        var annotationContainer = $get(annotationContainerPrefix + "EditorPanel");

        // Move active editor to annotation
        var bounds = Sys.UI.DomElement.getBounds(annotationElem);
        CCE_Editor.instance.setSize(bounds.width, Math.max(200, bounds.height));
        
        //this makes bug 104427	0908 Bidi: FF3: Wiki Editor: When edit the CC block, Editor body text not mirrored correct.	
        //annotationContainer.insertBefore(CCE_Editor.element, null);


        // Save original values
        CCE_OriginalTitle = $get(annotationContainerPrefix + "HeaderTitle").innerHTML;
        CCE_OriginalContent = $get(annotationPrefix + "editorData").value; //annotationElem.innerHTML;
        CCE_OriginalTags = $get(annotationPrefix + "tagEditorData").value;

        // Hide target annotation
        annotationElem.style.display = "none";
        $get(annotationContainerPrefix + "Header").style.display = "none";
        var historyElem = $get(annotationContainerPrefix + "HistoryContent");
        if (historyElem) {
            historyElem.style.display = "none";
        }
        $get(annotationContainerPrefix + "TagEditor_panel").style.display = "none";

        // Modify styles
        annotationContainer.className = "CCI_Text_Edit";
        var annotationPanelID = annotationContainerPrefix + "Container";
        CCE_RemoveMouseEffectFunc = function() { CCI_Highlight(false, annotationPanelID); };
        var annotationPanelElem = $get(annotationPanelID);
        $addHandler(annotationPanelElem, "mouseover", CCE_RemoveMouseEffectFunc);
        annotationPanelElem.className = "CCI_Container_Edit";
    }
    else { // User is editing a new annotation
        // Reset original values
        CCE_OriginalTitle = "";
        CCE_OriginalContent = "";
        CCE_OriginalTags = "";
    }

    // Show active editor
    CCE_Editor.containerElement.style.display = "";

    // Set editor values
    var titleElem = $get(CCE_TitleID);
    titleElem.value = CCE_OriginalTitle;
    CCE_SetEditorContent(CCE_OriginalContent);
    if (idx != -1) {
        annotationContainer.insertBefore(CCE_Editor.element, null);
    }
    $get(CCE_EditorPrefix + "tagEditorTextBox").value = CCE_OriginalTags;
    $get(CCE_MessageLabelID).style.display = "none";

    // Show UI
    titleElem.style.display = "";
    $get(CCE_ButtonsID).style.display = "";
    $get(CCE_LicenseID).style.display = "";
    CCE_Editor.containerElement.scrollIntoView(true);
    titleElem.focus();

    // Disable add link
    $get(CCS_AddLinkID).className = "CCS_AddLinkDisabled";

    CCE_IsEditing = true;
}

// Searches children for an element with the specified id
function CCE_GetChildElementByID(elem, id) {
    var childCount = elem.childNodes.length;
    for (var i = 0; i < childCount; i++) {
        var childElem = elem.childNodes[i];
        if (childElem.id == id) {
            return childElem;
        }
        if (childElem.childNodes.length > 0) { // Traverse all children
            var result = CCE_GetChildElementByID(childElem, id);
            if (result) {
                return result;
            }
        }
    }
    return null;
}

function CCE_GetControl(idx, name) {
    return $get(CCE_GetControlPrefixes(idx).control + name);
}

function CCE_GetControlPrefixes(idx) {
    var count = CCE_EditorInfo.length;
    for (var i = 0; i < count; i++) {
        var value = CCE_EditorInfo[i];
        if (value['i'] == idx) {
            return { control: value['p'], container: value['c'] };
        }
    }
}

function CCE_HideEditor(idx, annotationPrefix) {
    if (idx != -1) { // User was editing an existing annotation
        var annotationContainerPrefix = CCE_GetControlPrefixes(idx).container;
        var annotationElem = $get(annotationPrefix + "RadEditor");

        // Move shared editor back to bottom of page
        CCE_Editor.containerElement.insertBefore(CCE_Editor.element, null);

        // Show annotation
        annotationElem.style.display = "";
        $get(annotationContainerPrefix + "Header").style.display = "";
        var historyElem = $get(annotationContainerPrefix + "HistoryContent");
        if (historyElem) {
            historyElem.style.display = "none";
        }
        $get(annotationContainerPrefix + "TagEditor_panel").style.display = "";

        // Modify styles
        var annotationContainer = $get(annotationContainerPrefix + "EditorPanel");
        annotationContainer.className = "CCI_Text";
        var annotationPanelID = annotationContainerPrefix + "Container";
        var annotationPanelElem = $get(annotationPanelID);
        $removeHandler(annotationPanelElem, "mouseover", CCE_RemoveMouseEffectFunc);
        CCE_RemoveMouseEffectFunc = null;
        annotationPanelElem.className = "CCI_Container";
    }

    // Hide active editor
    CCE_Editor.containerElement.style.display = "none";

    // Hide new annotation UI
    $get(CCE_TitleID).style.display = "none";
    $get(CCE_ButtonsID).style.display = "none";
    $get(CCE_LicenseID).style.display = "none";

    // Enable add link
    $get(CCS_AddLinkID).className = "CCS_AddLink";

    CCE_Editor.tooltip.hide();
    CCE_IsEditing = false;
    CCE_CurrentAnnotation = -1;
}

function CCE_Init() {
    var prefixes = CCE_GetControlPrefixes(-1);
    CCE_EditorPrefix = prefixes.control;
    CCE_EditorContainerPrefix = prefixes.container;
    CCE_TitleID = CCE_EditorPrefix + "title";
    CCE_ButtonsID = CCE_EditorPrefix + "panButtons";
    CCE_LicenseID = CCE_EditorPrefix + "licensePanel";
    CCE_SubmitBtnID = CCE_EditorPrefix + "Save";
    CCE_CancelBtnID = CCE_EditorPrefix + "Cancel";
    CCE_MessageLabelID = CCE_EditorPrefix + "lblMessage";

    window.setTimeout(CCE_CheckCookies, 400);
}

function CCE_InitEditor() {
    if (CCE_Editor == null) {
        CCE_Editor =
    {
        instance: $find(CCE_EditorPrefix + "RadEditor"),
        tooltip: $find(CCE_EditorPrefix + "RadTooltip"),
        element: $get(CCE_EditorPrefix + "Container"),
        containerElement: $get(CCE_EditorContainerPrefix + "EditorPanel"),
        contentElement: $get(CCE_EditorPrefix + "RadEditorCenter")
    };
        // Handle tab order

        // Tab from title to editor
        $addHandler($get(CCE_TitleID), "keydown", function(e) {
            if (e.keyCode == 9
         && e.shiftKey == false) { // TAB:  Set focus to editor
                var de = new Sys.UI.DomEvent(e);
                de.preventDefault();
                de.stopPropagation();
                CCE_Editor.instance.setFocus();
            }
        }
    );
        $addHandler(CCE_Editor, "click", function() {
            CCE_Editor.instance.setFocus();
        }
    );
    }
}

function CCE_OnClientCmdExecuted(sender, args) {

    var cmdName = args.get_commandName();

    if (cmdName == "FormatBlock") {
        var val = args.get_value();
        var selElem = sender.getSelectedElement();

        if (val == "<body>") {
            if (selElem.tagName == "P") {
                selElem.removeAttribute("className");
                selElem.removeAttribute("class");
            }
        }
        else if (val.toLowerCase().indexOf("<pre") > -1) {
            var doc = sender.get_document();
            var isIE = sender.isIE;

            var pres = doc.getElementsByTagName("PRE");

            for (var i = 0; i < pres.length; i++) {
                var pre = pres[i];

                //MOZ - Keeps needless BR tags before and after PRE. Remove as well                
                if (!isIE) {
                    if (pre.previousSibling) {
                        var br = pre.previousSibling.previousSibling;
                        if (br && br.tagName == "BR") br.parentNode.removeChild(br);
                    }

                    if (pre.nextSibling) {
                        var br = pre.nextSibling.nextSibling;
                        if (br && br.tagName == "BR") br.parentNode.removeChild(br);
                    }

                    //Remove the starting \n that might exist
                    var textNode = pre.firstChild;
                    if (textNode && textNode.nodeType == 3 && textNode.nodeValue.startsWith("\n")) {
                        var newText = doc.createTextNode(textNode.nodeValue.trim());
                        textNode.parentNode.replaceChild(newText, textNode);
                    }
                }
            }
        }
    }
}


function OnClientCommandExecuting(editor, args) {
    //The command name
    var commandName = args.get_commandName();

    if (commandName === "Outdent" && typeof (args._tool) != "undefined") {
        args.set_cancel(true);
        editor.fire("Indent");
    }
    if (commandName === "Indent" && typeof (args._tool) != "undefined") {
        args.set_cancel(true);
        editor.fire("Outdent");
    }
}

function CCE_OnClientLoad(sender, args) {
    var editor_element = sender.get_element();

    // Hide unwanted areas
    var tableID = editor_element.id + 'Wrapper';
    var rows = $get(tableID).rows;
    rows[0].style.display = 'none';
    rows[3].style.display = 'none';
    rows[4].style.display = 'none';
    rows[5].style.display = 'none';

    // Add onkeydown handler
    var body = document.all ? sender.get_document().body : sender.get_document();
    $telerik.addExternalHandler(
             body,
             "keydown",
             function(e) {
                 if (e.keyCode == 9
                && e.shiftKey == true) { // <SHIFT>TAB: Set focus to title text box
                     var de = new Sys.UI.DomEvent(e);
                     de.preventDefault();
                     de.stopPropagation();
                     window.setTimeout(function() { $get(CCE_TitleID).focus(); }, 0);
                 }
             });
}

function CCE_SaveAnnotation() {
    // Check validity of content
    var titleElem = $get(CCE_TitleID);
    var title = titleElem.value;
    var content = CCE_Editor.instance.get_html(false);
    var tags = $get(CCE_EditorPrefix + "tagEditorTextBox").value;

    if (title.trim().length == 0) { // Title is empty
        CCE_ShowError(titleElem, CCE_ErrEmptyTitle);
        return;
    }

    if (title.length > CCE_TitleMaxLen) { // Title too long
        CCE_ShowError(titleElem, CCE_ErrTitleTooLong);
        return;
    }

    if (content.trim().length == 0
   || CCE_Editor.instance.get_text().trim().length == 0) { // Content is empty
        CCE_ShowError(CCE_Editor.contentElement, CCE_ErrEmptyContent);
        return;
    }

    if (content.length > CCE_ContentMaxLen) { // Content is too long
        CCE_ShowError(CCE_Editor.contentElement.body, CCE_ErrContentTooLong);
        return;
    }

    if ((content.trim() == CCE_OriginalContent.trim() || CCE_Editor.instance.get_html(true).trim() == CCE_OriginalContent.trim())
   && title.trim() == CCE_OriginalTitle.trim()
   && tags.trim() == CCE_OriginalTags.trim()) { // Content is unchanged
        CCE_ShowError(CCE_Editor.contentElement, CCE_ErrSameContent);
        return;
    }

    // Save the annotation
    var submitBtn = $get(CCE_SubmitBtnID);
    submitBtn.disabled = true;
    submitBtn.value = CCE_UpdateText + "...";
    $get(CCE_CancelBtnID).style.display = "none";
    Microsoft.Mtps.Web.WebServices.CCWebService.UpdateAnnotation(CCS_User,
                                                               CCS_ContentUrl,
                                                               CCE_CurrentAnnotation,
                                                               content,
                                                               title,
                                                               tags,
                                                               CCT_Browse,
                                                               CCT_Remove,
                                                               CCE_UpdateAnnotationComplete,
                                                               CCE_UpdateAnnotationFailure);
}

function CCE_SetHtml(target, content) {
    if (target) {
        var elem = document.createElement("div");
        if (content) {
            elem.innerHTML = content;
        }
        elem.style.display = "inline";
        target.innerHTML = "";
        target.appendChild(elem);
    }
}

function CCE_SetEditorContent(content) {
    // Set the content
    CCE_Editor.instance.set_html(content);

    // Clear redo/undo stacks
    var manager = CCE_Editor.instance.get_commandsManager();
    manager._clearCommandsToRedo();
    var length = manager.getCommandsToUndo().length;
    for (var i = 0; i < length; i++) {
        manager.removeCommandAt(0);
    }
}

function CCE_ShowError(target, text) {
    // Show tooltip
    CCE_Editor.tooltip.set_text(text);
    CCE_Editor.tooltip.set_targetControl(target);
    window.setTimeout(function() { CCE_Editor.tooltip.show(); }, 10);
}

function CCE_UpdateAnnotationComplete(result) {
    if (CCE_CurrentAnnotation != -1) {	// Update the existing annotation
        var prefixes = CCE_GetControlPrefixes(CCE_CurrentAnnotation);
        var annotationPrefix = prefixes.control;
        var annotationContainerPrefix = prefixes.container;
        var annotationElem = $get(annotationPrefix + "RadEditor");

        $get(annotationContainerPrefix + "HeaderTitle").innerHTML = result.Title;
        CCE_SetHtml(annotationElem, result.Content);
        $get(annotationPrefix + "editorData").value = result.Content;
        if (result.Tags != null) {
            $get(annotationPrefix + "tagEditorData").value = result.Tags;
        }
        var tagEditorBody = $get(annotationContainerPrefix + "TagEditor_tags");
        CCE_SetHtml(tagEditorBody, result.TagsHtml);
    }
    else {
        // Create new annotation
        var prefixes = CCE_GetControlPrefixes(-2);  // -2 is index for the template
        var templateContainerPrefix = prefixes.container;
        var templatePrefix = prefixes.control;
        var newContainerPrefix = CCE_EditorContainerPrefix + CCE_EditorInfo.length + "_";
        var newPrefix = CCE_EditorPrefix + CCE_EditorInfo.length + "_";
        var newAnnotation = $get(templateContainerPrefix + "Container").cloneNode(true);
        newAnnotation.style.display = "";

        // Get main template elements
        var newHeaderElem = CCE_GetChildElementByID(newAnnotation, templateContainerPrefix + "Header");
        var newEditorElem = CCE_GetChildElementByID(newAnnotation, templateContainerPrefix + "EditorPanel")
        var newTagEditorElem = CCE_GetChildElementByID(newAnnotation, templateContainerPrefix + "TagEditor_panel")

        // Assign new ids
        newAnnotation.id = newContainerPrefix + "Container";
        newHeaderElem.id = newContainerPrefix + "Header";
        newEditorElem.id = newContainerPrefix + "EditorPanel";
        CCE_GetChildElementByID(newEditorElem, templatePrefix + "Container").id = newPrefix + "Container";
        newTagEditorElem.id = newContainerPrefix + "TagEditor_panel";
        newTagEditorElem.className = "CCT_Panel_Block";

        // Title
        var titleElem = CCE_GetChildElementByID(newHeaderElem, templateContainerPrefix + "HeaderTitle");
        titleElem.id = newContainerPrefix + "HeaderTitle";
        titleElem.innerHTML = result.Title;

        // Content
        var contentElem = CCE_GetChildElementByID(newEditorElem, templatePrefix + "RadEditor");
        CCE_SetHtml(contentElem, result.Content);
        contentElem.id = newPrefix + "RadEditor";
        contentElem.title = "";
        var dataElem = CCE_GetChildElementByID(newAnnotation, templatePrefix + "editorData");
        dataElem.id = newPrefix + "editorData";
        dataElem.value = result.Content;

        // Tags
        var tagsElem = CCE_GetChildElementByID(newTagEditorElem, templateContainerPrefix + "TagEditor_tags");
        CCE_SetHtml(tagsElem, result.TagsHtml);
        var tagsDataElem = CCE_GetChildElementByID(newAnnotation, templatePrefix + "tagEditorData");
        tagsDataElem.id = newPrefix + "tagEditorData";
        if (result.Tags) {
            tagsDataElem.value = result.Tags;
        }

        // User
        var existinguser = $get(CCE_EditorContainerPrefix + "lnkViewProfile");
        if (existinguser != null) {
            var userLink = existinguser.cloneNode(true);
            userLink.className = "CCI_Header_Link";
            var newEditInfo = CCE_GetChildElementByID(newHeaderElem, templateContainerPrefix + "EditInfo");
            newEditInfo.appendChild(userLink);
        }

        // Links
        var editElem = CCE_GetChildElementByID(newHeaderElem, templateContainerPrefix + "Edit"); ;
        $addHandler(editElem,
                "click",
                function(e) { CCE_EditAnnotation(result.Index); e.stopPropagation(); e.preventDefault(); });
        $addHandler(contentElem,
                "dblclick",
                function(e) { CCE_EditAnnotation(result.Index); e.stopPropagation(); e.preventDefault(); });

        // Remove 'Add A Tag'
        CCE_GetChildElementByID(newTagEditorElem, templateContainerPrefix + "TagEditor_addTagLink").style.display = "none";

        // Add the annotation
        $get(CCE_EditorContainerPrefix + "wikiItems").appendChild(newAnnotation);
        CCE_EditorInfo[CCE_EditorInfo.length] = { i: result.Index, p: newPrefix, c: newContainerPrefix };
    }

    // Hide the editor
    CCE_HideEditor(CCE_CurrentAnnotation, CCE_GetControlPrefixes(CCE_CurrentAnnotation).control);

    // Show buttons
    var submitBtn = $get(CCE_SubmitBtnID);
    submitBtn.value = CCE_SubmitText;
    submitBtn.disabled = false;
    $get(CCE_CancelBtnID).style.display = "";
}

function CCE_UpdateAnnotationFailure(error) {
    var submitBtn = $get(CCE_SubmitBtnID);
    CCE_ShowError(submitBtn, CCE_ErrServerError);
    submitBtn.value = CCE_SubmitText;
    submitBtn.disabled = false;
    $get(CCE_CancelBtnID).style.display = "";
}
function licTooltipClientShow(sender, eventArgs) {
    var licTooltipId = "RadToolTipWrapper_" + Telerik.Web.UI.RadToolTip.getCurrent()._element.id;
    if ($get(licTooltipId).offsetLeft < 0)
        $get(licTooltipId).style.left = "0px";
    if (jQuery.browser.mozilla)
        $get(licTooltipId).style.top = $get(CCE_GetControlPrefixes(-1).control + "licensePreText").offsetTop - $get(licTooltipId).offsetHeight - 16 + "px";
}

function ErrorTooltipClientShow(sender, eventArgs) {
    var errorTooltipId = "RadToolTipWrapper_" + CCE_Editor.tooltip._element.id;
    if ($get(errorTooltipId).offsetLeft < 0)
        $get(errorTooltipId).style.left = CCE_Editor.element.offsetWidth / 2 + "px";
}



var _ccs_controlId='';
var _ccs_containerId='';
var _timer;

function CCS_BeginRequestHandler(sender, args)
{
  var prm = Sys.WebForms.PageRequestManager.getInstance();
  _ccs_controlId = args.get_postBackElement().id;

  // Prevent multiple requests
  if (prm.get_isInAsyncPostBack())
  {
    args.set_cancel(true);
  }

  // Display progress indicator
  var idx = _ccs_controlId.lastIndexOf('_');
  _ccs_containerId = _ccs_controlId.substring(0,idx+1) + 'Container';
  var container = $get(_ccs_containerId);
  if (container)
  {
    // Show progress indicator here
  }
}

function CCS_EndRequestHandler(sender, args)
{
  var prm = Sys.WebForms.PageRequestManager.getInstance();

  // Hide progress indicator here
  _ccs_controlId='';
  _ccs_containerId='';
}

function CCS_Scroll()
{
  // Reposition tooltip
  var tooltip = Telerik.Web.UI.RadToolTip.getCurrent();
  if (tooltip && tooltip.isVisible())
  {
    tooltip.show();
  }
}

function CCS_ShowHelp(path,width,height)
{
  var top = (window.screen.height/2)-(height/2);
  var left = (window.screen.width/2)-(width/2);
  window.open(path,'_blank','height='+height+',width='+width+',top='+top+',left='+left+',location=no,menubar=no,resizable=yes,status=no,toolbar=no,scrollbars=yes');
}




var _currentannotationIndex = 0;
var _flaggingContentBug = false;
var _unflaggingContentBug = false;
var _originatingUrl;

function CCT_AddATag(url,annotationIndex)
{
	if ( url != "" )
	{	// Need to authenticate
		document.cookie = "edit=" + annotationIndex;
		document.cookie = "scroll=" + annotationIndex;
		document.location.href = url;
	}
	else
	{	// Already authenticated
		CCT_ShowTagEditorEditPanel(annotationIndex);
		CCT_GetControl(annotationIndex,"textbox").focus();
	}
	return false;
}

function CCT_AddTags(annotationIndex)
{
  var value = CCT_GetControl(annotationIndex,"textbox").value;
  if ( value != "" )
  {
	  _currentannotationIndex = annotationIndex;
	  Microsoft.Mtps.Web.WebServices.CCWebService.AddTags(CCS_User,
                                                        _originatingUrl,
                                                        value,
                                                        annotationIndex,
                                                        CCT_Browse,
                                                        CCT_Remove,
                                                        CCT_TagEditorChangeComplete,
                                                        CCT_TagEditorChangeFailure);
	  CCT_ShowTagEditorViewPanel(annotationIndex);
  }
  else
  {
	  CCT_GetControl(annotationIndex,"messagePanel").innerHTML = CCT_NoTags;
  }
	return false;
}

function CCT_BrowseTag(url,annotationIndex)
{
	document.cookie = "scroll=" + annotationIndex;
	document.location.href = url;
	return false;
}

function CCT_CancelAddTag(annotationIndex)
{
  CCT_ShowTagEditorViewPanel(annotationIndex);
	return false;
}

function CCT_FlagContentBug(url,annotationIndex)
{
	if ( url != "" )
	{	// Need to authenticate
		document.cookie = "contentbug=" + annotationIndex;
		document.cookie = "scroll=" + annotationIndex;
		document.location.href = url;
	}
	else
	{	// Already authenticated
		_currentannotationIndex = annotationIndex;
    _flaggingContentBug = true;
		Microsoft.Mtps.Web.WebServices.CCWebService.AddTags(CCS_User,
                                                        _originatingUrl,
                                                        CCT_BugTag,
                                                        annotationIndex,
                                                        CCT_Browse,
                                                        CCT_Remove,
                                                        CCT_TagEditorChangeComplete,
                                                        CCT_TagEditorChangeFailure );
	}
	return false;
}

function CCT_GetAnnotationIndex(controlID)
{
  var count = CCT_Controls.length;
  for ( var i=0; i<count; i++ )
  {
    var value = CCT_Controls[i];
    var idx = controlID.indexOf(value["prefix"]);
    if ( idx != -1 )
    {
      return value["id"];
    }
  }
  return -1;
}

function CCT_GetControl(annotationIndex,control)
{
  var count = CCT_Controls.length;
  for ( var i=0; i<count; i++ )
  {
    var value = CCT_Controls[i];
    if ( value["id"] == annotationIndex )
    {
      return $get(value["prefix"]+control);
    }
  }
}

function CCT_GetTags(annotationIndex)
{
  var tags = "";
	var tagEditorBody = CCT_GetControl(annotationIndex,"tags");
	if ( tagEditorBody )
	{
	  var childElems = tagEditorBody.children;
	  if ( childElems.length > 0 )
	  {
      // Deal with DIV that is inserted when a tag is added from client
	    if ( childElems[0].tagName == "DIV" )
	    {
	       childElems = childElems[0].children;
	    }

	    if ( childElems.length > 0 )
	    {
        // Get tag values from spans (each has two hyperlinks - we need the first)
	      for ( var i=0; i<childElems.length; i++)
	      {
	        var elem = childElems[i].children[0];
	        if ( elem.tagName == "A" )
	        {
            tags += elem.innerHTML + " ";
          }
	      }
	    }
	  }
	}
  return tags;
}

function CCT_Init(auth,originatingUrl)
{
	_originatingUrl = originatingUrl;

  // Check cookies
	var cookieParts = document.cookie.split("; ");
	var crumb="";
	for (var i=0; i<cookieParts.length; i++)
	{
		var crumbs = cookieParts[i].split("=");
		if (crumbs[0] == "scroll"
		 && crumbs[1] != "")
		{
			CCT_ScrollToTagEditor(crumbs[1]);
		}
		if (crumbs[0] == "edit"
		 && crumbs[1] != ""
		 && auth == "true")
		{
			CCT_ShowTagEditorEditPanel(crumbs[1]);
		}
	}
	CCT_RemoveTagEditorCookie("remove");
  CCT_RemoveTagEditorCookie("contentbug");
}

function CCT_KeyDown(evt)
{
  if ( evt.keyCode == 13 )
  { // Enter pressed - add the tags
    CCT_AddTags(CCT_GetAnnotationIndex(evt.target.id));
    evt.preventDefault();
	}
	else if ( evt.keyCode == 27 )
	{ // Escape pressed - cancel the operation
	  CCT_CancelAddTag(CCT_GetAnnotationIndex(evt.target.id));
    evt.preventDefault();
	}
	else
	{
	  return true;
	}
}

function CCT_RemoveTag(tag,url,annotationIndex)
{
	if ( url != "" )
	{	// Need to authenticate
		document.cookie = "remove=" + tag + ":" + annotationIndex;
		document.cookie = "scroll=" + annotationIndex;
		document.location.href = url;
	}
	else
	{	// Already authenticated
		_currentannotationIndex = annotationIndex;
		if (tag == CCT_BugTag)
		{
		  _unflaggingContentBug = true;
		}
		Microsoft.Mtps.Web.WebServices.CCWebService.RemoveTag(CCS_User,
                                                          _originatingUrl,
                                                          tag,
                                                          annotationIndex,
                                                          CCT_Browse,
                                                          CCT_Remove,
                                                          CCT_TagEditorChangeComplete,
                                                          CCT_TagEditorChangeFailure );
	}
	return false;
}

function CCT_RemoveTagEditorCookie(name)
{
	document.cookie = name + "=; expires=Fri, 02-Jan-1970 00:00:00 GMT";
}

function CCT_ScrollToTagEditor(annotationIndex)
{
	var ctrl = CCT_GetControl(annotationIndex,"textbox");
	if (ctrl)
	{
	  ctrl.scrollIntoView(false);
	}
	CCT_RemoveTagEditorCookie("scroll");
}

function CCT_ShowTagEditorEditPanel(annotationIndex)
{
	CCT_GetControl(annotationIndex,"view").style.display = "none";
	CCT_GetControl(annotationIndex,"editPanel").style.display = "";
	CCT_GetControl(annotationIndex,"textbox").value = "";
	CCT_GetControl(annotationIndex,"messagePanel").innerHTML = "";
	CCT_RemoveTagEditorCookie("edit");
}

function CCT_ShowTagEditorViewPanel(annotationIndex)
{
	CCT_GetControl(annotationIndex,"view").style.display = "";
	CCT_GetControl(annotationIndex,"editPanel").style.display = "none";
	CCT_GetControl(annotationIndex,"messagePanel").innerHTML = "";
}

function CCT_TagEditorChangeComplete(result)
{
	var elem = document.createElement( "div" );
	elem.innerHTML = result;
	elem.style.display = "inline";
	var tagEditorBody = CCT_GetControl(_currentannotationIndex,"tags");
	if ( tagEditorBody )
	{
	  tagEditorBody.innerHTML = "";
	  tagEditorBody.appendChild( elem );
	}
	if ( _flaggingContentBug )
	{
    _flaggingContentBug = false;
  	var flagPanel = CCT_GetControl(_currentannotationIndex,"flagAsPanel");
  	if ( flagPanel )
  	{ // Hide "Flag as content bug" UI
  	  flagPanel.style.display = "none";
  	}
  }
	if ( _unflaggingContentBug )
	{
    _unflaggingContentBug = false;
  	var flagPanel = CCT_GetControl(_currentannotationIndex,"flagAsPanel");
  	if ( flagPanel )
  	{ // Show "Flag as content bug" UI
  	  flagPanel.style.display = "block";
  	}
  }
	_currentannotationIndex = 0;
}

function CCT_TagEditorChangeFailure(error)
{
	CCT_GetControl(_currentannotationIndex,"messagePanel").innerHTML = CCT_Error;
	CCT_GetControl(_currentannotationIndex,"messagePanel").title = error.get_message();
	if ( _flaggingContentBug )
	{
    _flaggingContentBug = false;
  }
	if ( _unflaggingContentBug )
	{
    _unflaggingContentBug = false;
  }
}

function CCTagResizableAreaChange(isCollapsing) {
    resizeTagWidth();
}

function resizeTagWidth() {
    if (jQuery(".resizableArea .leftSection").length > 0 && jQuery(".CCT_Panel").length > 0 && jQuery.browser.msie && jQuery.browser.version == "7.0" && document.body.dir == "rtl") {
        if (jQuery(".resizableArea .leftSection")[0].style.display == "none")
            jQuery(".CCT_Panel")[0].style.width = jQuery(".CCS_TopicTagEditor")[0].offsetWidth - 10 + "px";
        else
            jQuery(".CCT_Panel")[0].style.width = "";
    }  
}

jQuery(document).ready(function() {
    if (jQuery(".resizableArea .leftSection").length > 0 && jQuery(".CCT_Panel").length > 0 && jQuery.browser.msie && jQuery.browser.version == "7.0" && document.body.dir == "rtl") {
        resizeTagWidth();
    }
}); 





function IsPageRated(objID, labelID, text, flyoutID, btn) {
    obj = document.getElementById(objID);
    if (obj.Rated) {
        SubmitRating(RatingObjId, sendBtnId, pageTag);
        window.onbeforeunload = function() {
        if (window.onbeforeunload_autosubmit)
            window.onbeforeunload_autosubmit();
        }
        flyout = document.getElementById(flyoutID)
        TFly_Popup(flyout, false);
        window.setTimeout(function() { btn.disabled = true }, 100);
        contentRated = true;
        obj.Rated = true;


        var idePrefix = objID.replace("Rating1", "");
        var rtgContainer = document.getElementById(idePrefix + "rtgContainer");

        obj.title = RatedTooltipText;
        if (rtgContainer != null) {
            rtgContainer.title = RatedTooltipText;
        }
        var mini_rightDivId = document.getElementById(flyoutID).parentNode.id;
        jQuery('#' + idePrefix + 'rtgContainer').appendTo('#' + mini_rightDivId);
        flyout.style.display = 'none';
        for (var i = 1; i <= maxRating; i++) {
            var star = document.getElementById(objID + "_Star_" + i);
            if (star != null) {
                star.title = RatedTooltipText;
            }
        }
        return true;
    }
    else {
        label = document.getElementById(labelID)

        if (label.timer) {
            window.clearTimeout(label.timer)
        } else {
            label.className_ = label.className;
            label.innerHTML_ = label.innerHTML;
            label.className = 'WarningMessage';
            label.innerHTML = text;
        }

        label.timer = window.setTimeout(function() {
            label.className = label.className_;
            label.innerHTML = label.innerHTML_;
            label.timer = null;
        }
                      , 3000);
    }
    return false;
}
function GetRating() {
    var i = 1;
    while (i <= maxRating) {
        var obj;        
        if (MtpsPageDirection == "rtl")
            obj = document.getElementById(RatingObjId + "_Star_" + (maxRating + 1 - i));
        else 
            obj = document.getElementById(RatingObjId + "_Star_" + i);
        if (obj != null && obj.className.indexOf("emptyRatingStar") != -1)
            break;
        i++;
    }
    return i - 1;
}

function getComment() {
    var obj = document.getElementById(tbCommentsId);
    if (obj != null) {
        if(typeof obj.value != "undefined")
            return escapeHTML(obj.value);
        else return "";
    }
}
function escapeHTML(str) {
    var div = document.createElement('div');
    var text = document.createTextNode(str);
    div.appendChild(text);
    return div.innerHTML;
};
function tbNavParent(navUrl) {
    window.top.location.href = navUrl;
}

function SubmitRating(objName, btnName, tag) {
    try {
        // call web service
        var params = '{';
        params += '"guestId":"' + CurUserId + '"';
        params += ',"contentUrl":"' + ClientURL + '"';
        params += ',"device":"' + Rating_Device + '"';
        params += ',"userIPAddress":"' + userIPAddress + '"';
        params += ',"userRating":"' + GetRating() + '"';
        params += ',"userComments":"' + getComment() + '"';
        params += ',"logException":"' + MTPS.Controls.MtpsRating.EnableLogException + '"';
        params += '}';

        jQuery.ajax({
            type: "POST",
            url: "/Platform/Controls/InteractiveToolbar/RatingsWebService.asmx/RateTopic",
            data: params,
            contentType: "application/json; charset=utf-8",
            dataType: "json",
            async: false,
            success: function(msg) {
                //alert(msg.d);
            },
            error: function(response) {
                if(MTPS.Controls.MtpsRating.EnableLogException)
                    LogRateClientException(response.responseText);
            }
        });
        contentRated = true;
        
        MTPS.Controls.MtpsRating.ConvertWaitingToFilled();

        var obj = $get(objName)
        var btn = document.getElementById(btnName)
        if (obj && obj.Rated && btn) {
            var d = new Date();
            d.setFullYear(d.getFullYear() + 1)
            var CookieInfo = tag + "=" + escape("xxx") + ";expires=" + d.toGMTString() + " path=/;";
            document.cookie = CookieInfo;
        }
    } catch (e) {
        if(MTPS.Controls.MtpsRating.EnableLogException)
            LogRateClientException(e.message);
    };
}
function LogRateClientException(errorMsg) {
    var params = '{';
    params += '"contentUrl":"' + ClientURL + '"';
    params += ',"userAgent":"' + navigator.userAgent + '"';
    params += ',"errorMsg":"' + errorMsg + '"';
    params += ',"userIPAddress":"' + userIPAddress + '"';
    params += '}';
    
    jQuery.ajax({
        type: "POST",
        url: "/Platform/Controls/InteractiveToolbar/RatingsWebService.asmx/LogRateClientException",
        data: params,
        contentType: "application/json; charset=utf-8",
        dataType: "json",
        success: function(msg) { }
    });
}

function RegisterAutoSubmit(objName, btnName, tag) {
    window.onbeforeunload_autosubmit = window.onbeforeunload;
    window.onbeforeunload = function() {
        if (window.onbeforeunload_autosubmit)
            window.onbeforeunload_autosubmit();
        obj = document.getElementById(objName);
        if (obj != null && obj.Rated && !contentRated) {
            SubmitRating(objName, btnName, tag);
        }
    }
}

function SubmitRateAction(eventArgument) {
    try {
        // call web service
        var params = '{';
        params += '"guestId":"' + CurUserId + '"';
        params += ',"contentUrl":"' + ClientURL + '"';
        params += ',"userIPAddress":"' + userIPAddress + '"';
        params += ',"eventArgument":"' + eventArgument + '"';
        params += '}';

        jQuery.ajax({
            type: "POST",
            url: "/Platform/Controls/InteractiveToolbar/RatingsWebService.asmx/RateAction",
            data: params,
            contentType: "application/json; charset=utf-8",
            dataType: "json",
            success: function(msg) {
                //alert(msg.d);
            }
        });
    } catch (e) { };
}

function TFlyPreAnimate(TFly) {
    TFly.status = "on"
}

function CloseFLY(flyoutID) {
    var TFly1 = $get(flyoutID);
    TFly_SetVisibility(TFly1, false);
    document.body.focus();
}



if (typeof MTPS == "undefined" || !MTPS)
    var MTPS = {};
if (typeof MTPS.Controls == "undefined" || !MTPS.Controls)
    MTPS.Controls = {};
if (typeof MTPS.Controls.MtpsRating == "undefined" || !MTPS.Controls.MtpsRating)
    MTPS.Controls.MtpsRating = {};

MTPS.Controls.MtpsRating.createMtpsRating = function(id, _starCssClass, _filledStarCssClass, _emptyStarCssClass, _waitingStarCssClass, _maxRating, _direction, _enableLogException) {
    this.MtpsRatingSelector = '#' + id;
    this.MtpsRating = jQuery(this.MtpsRatingSelector);
    this.starCssClass = _starCssClass;
    this.filledStarCssClass = _filledStarCssClass;
    this.emptyStarCssClass = _emptyStarCssClass;
    this.waitingStarCssClass = _waitingStarCssClass;
    this.readOnly = false;
    // curRating: returns from Sys.Topics.Current.GetRating() in InteractiveToolbar.ascx.cs
    this.maxRatingValue = 5;
    this.currentRating = curRating;
    this.keyboardEditing = false;
    this.tag = "";
    this.stars = [];
    this.Rated = contentRated;
    this.EnableLogException = _enableLogException;
    if (this.Rated) {
        id = id.replace("_Flyout", "");
        document.getElementById(id + "_A").title = RatedTooltipText;
        this.readOnly = true;
    }
    else {
        document.getElementById(id + "_A").title = RatingTooltipText;
    }
    document.getElementById(id).Rated = this.Rated;
    if (_direction.toLowerCase().indexOf('lefttoright') != -1) {
        this.ratingDirection = 0;
    }
    else {
        this.ratingDirection = 1;
    }
    jQuery(this.MtpsRatingSelector + ' a').bind("onmouseout", function(e) {
        MTPS.Controls.MtpsRating.mouseOutHandler();
    });
    jQuery(this.MtpsRatingSelector + ' span').each(function(i) {
        if (MTPS.Controls.MtpsRating.ratingDirection == 0)
            jQuery(this).attr('value', i + 1);
        else
            jQuery(this).attr('value', MTPS.Controls.MtpsRating.maxRatingValue - i);
        jQuery(this).attr('title', jQuery(MTPS.Controls.MtpsRating.MtpsRatingSelector + ' a').attr('title'));
        var v = jQuery(this).attr('value');
        jQuery(this).bind("mouseover", function(e) {
            MTPS.Controls.MtpsRating.starMouseOverHandler(v);
        });
        jQuery(this).bind("click", function(e) {
            MTPS.Controls.MtpsRating.starClickHandler(v);
        });
        Array.add(MTPS.Controls.MtpsRating.stars, this);
    });

    this.starClickHandler = function(value) {
        if (this.readOnly || contentRated) {
            return;
        }
        var flyout = document.getElementById(RatingFlyoutID);
        if (flyout != null) {
            flyout.status = null;
            flyout.onPreAnimate = null;
            TFly_Popup(flyout, true)
        }

        this.set_Rating(value);
    };

    this.starMouseOverHandler = function(value) {
        if (this.readOnly || this.Rated) {
            return;
        }
        this.UpdateStarCSS(value, this.filledStarCssClass);
    };
    this.mouseOutHandler = function() {
        if (this.readOnly || this.Rated) {
            return;
        }
        this.UpdateStarCSS(this.currentRating, this.filledStarCssClass);
    };

    this.ConvertWaitingToFilled = function() {
        jQuery("." + this.waitingStarCssClass).removeClass(this.waitingStarCssClass).addClass(this.filledStarCssClass)
    }
    this.UpdateStarCSS = function(value, updateStarCSS) {
        jQuery(".ratingStar").removeClass(this.filledStarCssClass).removeClass(this.UpdateStarCSS).removeClass(this.waitingStarCssClass);
        for (var i = 0; i < this.maxRatingValue; i++) {
            var starElement;
            if (this.ratingDirection == 0) {
                starElement = this.stars[i];
            } else {
                starElement = this.stars[this.maxRatingValue - i - 1];
            }

            if (value > i) {
                starElement.className = this.starCssClass + " " + updateStarCSS;
            }
            else {
                starElement.className = this.starCssClass + " " + this.emptyStarCssClass;
            }
        }
    };

    this.set_Rating = function(value) {
        if (this.readOnly) {
            return;
        }
        this.UpdateStarCSS(value, this.waitingStarCssClass);
        this.Rated = true;
        document.getElementById(id).Rated = this.Rated;
    };

    this.OnKeyPress = function(e) {
        
        if (e.target.tagName.toLowerCase() == "input"
           || e.target.tagName.toLowerCase() == "textarea"
           || contentRated)
            return;
        if (e.which == 9) {
            if (!e.data.keyboardEditing) {
                document.getElementById(id).focus();
                e.data.keyboardEditing = true;
            }
            else {
                e.data.keyboardEditing = false;
                var commentid = jQuery("div[class='ratingFlyoutPopup'] textarea").attr('id');
            }
        }
        if (e.which == 37 && e.data.currentRating > 1 && e.data.keyboardEditing) {
            e.data.currentRating--;
            e.data.starClickHandler(e.data.currentRating);
        }
        if (e.which == 39 && e.data.currentRating < 5 && e.data.keyboardEditing) {
            e.data.currentRating++;
            e.data.starClickHandler(e.data.currentRating);
        }
        e.cancelBubble = true;
    };

    jQuery(this.MtpsRatingSelector).bind('keydown', this, this.OnKeyPress);

}



var b = window.navigator.appName;

//	Show the MT view selection drop down
function MTVDD_DropDownOn(e)
{   
	try
	{
		//	Change Image
		MTVDD_ChangeImage(true);
		//	Set Position
		MTVDD_PopUpDiv.style.top = MTVDD_ImgArrow.height;
		
		//	Add Event Handlers
		if(MTVDD_PopUpDiv.style.display != "inline")
		{
			MTVDD_PopUpDiv.style.display = "inline";

            if(document.addEventListener)
            {       //  Commented for # 32609
				    //  document.addEventListener("keydown", MTVDD_HandleEvent, false);
				    document.addEventListener("click", MTVDD_TestForResizableAreaControl, false);
				    window.addEventListener("resize", MTVDD_TestForResizableAreaControl, false);			
            } 
            else
            { 
				    document.attachEvent("onkeydown", MTVDD_HandleEvent, false);
       				document.body.onclick = MTVDD_HideFromClick;	
            } 

		}
		else
		{
			//	MTVDD_PopUpDiv.style.display = "none";
			setTimeout(MTVDD_HideThisMenu, 0);			
		}
		
	}
	catch(err)
	{
		throw err;
	}
	e.cancelBubble = true;
	MTVDD_Div.normalize();
	return;
}

// Handle click on view preference link
function MTVDD_SelectViewPreference(e, viewPreferenceCode) 
{
    setTimeout(MTVDD_HideThisMenu, 0);
    MTVDD_SetViewPreference(viewPreferenceCode);
    MTVDD_SaveViewPreference(viewPreferenceCode);
    window.location.reload();
    e.returnValue = false;
}

// Set current view preference
function MTVDD_SetViewPreference(viewPreferenceCode)
{
    MTVDD_CurrentViewPreference = viewPreferenceCode;
    var opt = document.getElementById("MtViewDropDownOption" + viewPreferenceCode.toString());
    MTVDD_PanelLink.innerHTML = MTVDD_DropDownLabel + " " + opt.innerHTML;
    MTVDD_PanelLink.title = opt.title;
}

// Handle the enter key for a section of a form, binding it to the provided submit buton 
function MTVDD_HandleEvent(event)
{ 
    var nav = window.Event ? true : false; 
    if (nav) { 
        return MTVDD_NetscapeEventHandler(event); 
    } else { 
        return MTVDD_MicrosoftEventHandler(); 
    } 
} 

function MTVDD_NetscapeEventHandler(e)
{ 
//trap for enter (13), escape (27) and "t" (84)
    if ((e.which == 13 || e.which == 27 || e.which == 84) && e.target.type != 'textarea' && e.target.type != 'submit')
     { 
		setTimeout(MTVDD_HideThisMenu, 0);
		document.removeEventListener("keydown", MTVDD_HandleEvent, false);
		e.cancelBubble = true;
        e.returnValue = false; 
        e.cancel = true; 
        e.preventDefault(); 
    } 
    return; 
} 

function MTVDD_MicrosoftEventHandler() { 
//trap for enter (13), escape (27) and "t" (84)
    if ((event.keyCode == 13 || event.keyCode == 27 || event.keyCode == 84 )&& event.srcElement.type != 'textarea' && event.srcElement.type != 'submit') { 
        event.returnValue = false; 
        event.cancel = true;
    	setTimeout(MTVDD_HideThisMenu, 0);
    	document.detachEvent("onkeydown", MTVDD_HandleEvent, false);
    	event.cancelBubble = true;
    } 
    return; 
}


//	Only used by IE
function MTVDD_HideFromClick(event)
{
	var objClicked = window.event.srcElement;
	var objParent = objClicked.parentNode.parentNode;
	
	if (objParent.id != MTVDD_PopUpDiv.id && objParent.id != MTVDD_Div.id ) 
	{
		setTimeout(MTVDD_HideThisMenu, 0);
		return;
	}
	else
	{
		window.event.cancelBubble = true;
		return;
	}
}

/// FF & NN only
function MTVDD_TestForResizableAreaControl(e)
{
	try
	{
	//	For keydown events
	if (e.type == "keydown")
	{
		//	alert(e.which);
		if (e.which == 84 || e.which == 13)
		{
			setTimeout(MTVDD_HideThisMenu, 0);
			document.removeEventListener("keydown", MTVDD_TestForResizableAreaControl, false);
			e.cancelBubble = true;
			return;
		}
	}
	else if (e.type == "resize")
	{
		setTimeout(MTVDD_HideThisMenu, 0);
		window.removeEventListener("resize", MTVDD_TestForResizableAreaControl, false);
		e.cancelBubble = true;
		return;		
	}
	
	// for mousedown event
		if (e.type == "click")
		{
			if (e.which == 1 || e.which == 32)
			{	
				var eNode = e.target;
				if (eNode.id.length == 0)
				{	//	alert("returning false");
					setTimeout(MTVDD_HideThisMenu, 0);
					document.removeEventListener("click", MTVDD_TestForResizableAreaControl, false);
					return false;
				}
				else
				{
					var pNode = document.getElementById(eNode.id).parentNode.parentNode;
					if (eNode.id != MTVDD_PopUpDiv.id)
					{
						if (pNode.id != MTVDD_PopUpDiv.id)
						{
							setTimeout(MTVDD_HideThisMenu, 0);
							document.removeEventListener("mousedown", MTVDD_TestForResizableAreaControl, false);
						}
					}			
					if (e.target.id == MTVDD_ImgArrow.id)
					{
						setTimeout(MTVDD_HideThisMenu, 0);
						document.removeEventListener("mousedown", MTVDD_TestForResizableAreaControl, false);
					}
					if (e.target.id == MTVDD_PanelLink.id)
					{
						setTimeout(MTVDD_HideThisMenu, 0);
						document.removeEventListener("mousedown", MTVDD_TestForResizableAreaControl, false);
					}
				}
				return;
			}
		}
	}
	catch(err)
	{
		throw err;
	}
}

// Persist Menu long enough for client to click check boxes
function MTVDD_HideThisMenu()
{
	MTVDD_PopUpDiv.style.display = "none";
	MTVDD_ImgArrow.src = MTVDD_ArrowOffPath;
	document.body.onclick = null;
	return;
}

//	Handle Image changes
function MTVDD_ChangeImage(boolOnOff)
{
	if (boolOnOff == true)
	{
		MTVDD_ImgArrow.src = MTVDD_ArrowOnPath;
	}
	else
	{
		MTVDD_ImgArrow.src = MTVDD_ArrowOffPath;
	}
}

// Save view preference in the "MtViewPreference" cookie.
function MTVDD_SaveViewPreference(viewPreferenceCode)
{
	if (window.navigator.cookieEnabled == true)
	{
        var daysToExpire = 60;
        var cookieDate = new Date();
	    cookieDate.setTime(cookieDate.getTime()+(daysToExpire*24*360000));
	    document.cookie = "MtViewPreference=" + viewPreferenceCode.toString() + "; expires=" + cookieDate.toGMTString();
	}
}




function jsTrim(s) {return s.replace(/(^\s+)|(\s+$)/g, "");}

function Track(trackParm, objLink) {
    var daysToExpire = 60;
    var cookieDate = new Date();
    cookieDate.setTime(cookieDate.getTime() + (daysToExpire * 24 * 2160000000));
    expires = "expires=" + cookieDate.toGMTString();
    if (typeof (pageShortId) !== "undefined" && objLink.hash) {
        document.cookie = "InstrumentedLinkAnchorHashCookie=" + pageShortId + ":" + objLink.hash.replace("#","") + "; " + expires;
    }
    
    if (objLink.innerText && jsTrim(objLink.innerText))
    {
        // anchor tag, use link text
        LinkText = objLink.innerText;
    }
    else if (objLink.all && objLink.all(0).alt)
    {
        // image, use alt text
        LinkText = objLink.all(0).alt;
    }
         
    var strDomain = document.domain;
    var s = trackParm.split("|");
    var strLinkArea;
    var strLinkId;
    var strPageRegion;
    
    if (trackParm.substring(0,1) == "|")
    {
        strPageRegion = s[1];
        strLinkArea = s[2];
        strLinkId = s[2] + s[3];
    }
    else
    {
            strLinkArea = s[0];
        strLinkId = s[0] + s[1];

    }
    
    if (typeof(LinkText)=="undefined" || !LinkText || LinkText == "")
        LinkText = strLinkId;

    ctUrl = objLink.href + "?LinkId=" + strLinkId + "&LinkArea=" + strLinkArea 

    if (typeof(DCSext)!="undefined") 
    {
            DCSext.wt_strHeadlnLocale = detectedLocale;
            DCSext.wt_strCat=strLinkArea+"|"+detectedLocale;
            DCSext.wt_strUrl = window.location.href.toLowerCase();
            DCSext.wt_strArea = strPageRegion;
    }
        return false; 
}



// First events
function FlyoutPopupShow() {
    jQuery(".LocaleManagementFlyoutPopup").toggle();

}

function FlyoutPopupHide() {
    if (jQuery('.LocaleManagementFlyoutPopup').css('display') == 'block')

        jQuery(".LocaleManagementFlyoutPopup").toggle();

}




var MtpsLoginStatus={};
MtpsLoginStatus.SignInHappened = 0;
MtpsLoginStatus.LoginWindowHandle = 0;
MtpsLoginStatus.OverlayImagePath = "/Platform/Controls/StoMtpsLoginStatus/resources/veil.gif";

MtpsLoginStatus.DoPassportSignIn = function(url) 
{
    if (MtpsLoginStatus.SignInHappened < 1) 
    {    
        var e = (screen.availWidth - 850) * .5;
        var f = (screen.availHeight - 450) * .5;
        var d = "channelmode=0,directories=0,fullscreen=0,location=1,menubar=0,resizable=0,scrollbars=1,status=0,titlebar=0,toolbar=0,copyhistory=0,statusbar=0";

        MtpsLoginStatus.LoginWindowHandle = MtpsLoginStatus.OpenNewWindow(url, "_blank", d, 900, 500, e, f);

        MtpsLoginStatus.LoginWindowHandle.focus();
        MtpsLoginStatus.SignInHappened++;
        MtpsLoginStatus.ExpandOrCollapse("idPPMWOverlay");

        setTimeout("MtpsLoginStatus.CheckOnPassportSignIn()", 100);

        return false;
    }
    else 
    {
        return true;
    }
};

MtpsLoginStatus.CheckOnPassportSignIn = function()
{
	if (MtpsLoginStatus.LoginWindowHandle.closed)
	{
		MtpsLoginStatus.ExpandOrCollapse("idPPMWOverlay");
	}
	else
	{
		setTimeout("MtpsLoginStatus.CheckOnPassportSignIn()",100);
	}
};
	
MtpsLoginStatus.OpenNewWindow = function(m,j,d,f,e,a,b)
{
	var g = "toolbar, location, directories, status, menubar, scrollbars, resizable";
	var i = "width="+(f>0?f:GetWindowWidth())+", ";
	var h = "height="+(e>0?e:GetWindowHeight())+", ";
	
	a = a ? a : 0;
	
	var k = "screenX=" + a + ", left=" + a + ", ";
	
	b= b ? b : 0;
	
	var l = "screenY=" + b + ", top=" + b;
	
	var c = window.open(m,j,(d=="all"?g:d)+", "+i+h+k+l);
	
	if (c != null)
		c.focus();
   
	return c
};

MtpsLoginStatus.ExpandOrCollapse = function()
{
	for (var i=0; i<MtpsLoginStatus.ExpandOrCollapse.arguments.length; i++)
	{
		var element = document.getElementById(MtpsLoginStatus.ExpandOrCollapse.arguments[i]);
		element.style.display = (element.style.display == "none") ? "block" : "none";
		element.style.background = (element.style.background == "") ? "url(" + MtpsLoginStatus.OverlayImagePath + ") repeat" : "";
	}
};




function dolivesearch(sender) {
    var o = document.getElementById(sender);
    if (o.className == "TextBoxSearch") {
        document.getElementById("go").value = "false";
    }
    else {
        document.getElementById("go").value = "true";
        if (typeof dcsMultiTrack == "function") {
            var dcsuri, dcssip, ti;
            dcsuri = "results.aspx";
            dcssip = "search.live.com";
            ti = "_STO_SearchButton: Web";
            dcsMultiTrack("DCS.dcsuri", dcsuri, "DCS.dcssip", dcssip, "WT.ti", ti, "WT.dl", "2", "WT.z_srch_t", "", "DCS.dcsqry", "", "WT.ad", "", "WT.mc_id", "");
            DCS.dcsuri = DCS.dcssip = DCS.dcsqry = WT.ti = WT.z_srch_t = "";
        } 
    }
}





var control;
(function($) {
    $(".internav .normal").hover(
        function() {
            $(".Tab_Left", this).addClass("Tab_Left_Hovered");
            $(".Tab_Center", this).addClass("Tab_Center_Hovered");
            $(".Tab_Right", this).addClass("Tab_Right_Hovered");
        },
        function() {
            $(".Tab_Left", this).removeClass("Tab_Left_Hovered");
            $(".Tab_Center", this).removeClass("Tab_Center_Hovered");
            $(".Tab_Right", this).removeClass("Tab_Right_Hovered")
        }
    );
})(jQuery);



function TFly_Init(name,  offsetLeft, offsetTop, alignment, anyIter, anyTime, staticHover, popupHover, popupLatency, hideLatency, onInit, onPreAnimate, onPopup, onHide){
  var TFly = document.getElementById(name); 
  TFly.alignment = alignment;
  TFly.DT = document.getElementById(name+"_Popup")
  TFly.F = document.getElementById(name + "_Anim")
  TFly.S = TFly.firstChild;
  
  
  TFly.oX = offsetLeft
  TFly.oY = offsetTop;

  TFly.AnyIter = anyIter;
  TFly.AnyTime = anyTime;

  TFly.popupLatency = popupLatency;
  TFly.hideLatency = hideLatency;
  TFly.onPopup = onPopup;
  TFly.onPreAnimate = onPreAnimate;
  TFly.onHide = onHide;
  
  TFly.IsHover=false;
  TFly.InLoadingState=false;
  
  TFly.className_ = TFly.className;
  TFly.DT.className_=TFly.DT.className
  
  TFly.onmouseover = function(){TFly.IsHover=true; if(staticHover!=''){TFly.className=staticHover}; TFly_Popup(TFly,true);}
  TFly.onmouseout = function() { if (staticHover != '') { TFly.className = TFly.className_ }; TFly_Popup(TFly, false); TFly.DT.blur(); TFly.IsHover = false; }

  if(document.all){
    TFly.onactivate = function(){if(!TFly.IsHover)TFly.onmouseover();}
    TFly.ondeactivate = function(){TFly.onmouseout();}
  }else{
    TFly.onfocus = function(){TFly.onmouseover()}
    TFly.onblur = function(){TFly.onmouseout()}
  }
 
 
  TFly.DT.onmouseover = function(){ TFly.IsHover=true; if(popupHover!=''){TFly.DT.className=popupHover;};TFly_Popup(TFly,true);}
  TFly.DT.onmouseout = function(){ if(popupHover!=''){TFly.DT.className=TFly.DT.className_};TFly_Popup(TFly,false);TFly.IsHover=false; }

  TFly_CallClientFunction(onInit, TFly)
}

function TFly_Reposition(TFly){
  var clientWidth = document.body.scrollWidth - 20; //scroller width
  var width = TFly.DT.offsetWidth;
  var absX = TFly.DT.offsetLeft 
  var d = TFly.DT
  while(d = d.offsetParent)
    absX += d.offsetLeft

  if(clientWidth<(absX+width)){
   var diff = clientWidth - absX -width
   TFly.DT.style.left = TFly.DT.offsetLeft + diff + "px"
  }
}

function TFly_CallClientFunction(name, TFly){
    if(name!=null &&  name!="")
      eval( name + "(TFly);");
}

function TFly_SetPosition(TFly, obj){
  obj.style.top = TFly.oY + TFly.offsetHeight+ "px";
  obj.style.left = TFly.oX + (TFly.offsetWidth - obj.offsetWidth )*(1+TFly.alignment)/2 + "px";
}

function TFly_Popup(TFly,value){

  if (TFly.onPreAnimate)
  {
    TFly_CallClientFunction(TFly.onPreAnimate, TFly)
  }
  if(TFly.timer!=null ) window.clearTimeout(TFly.timer)
  if(value){
    if(TFly.status=="on") return
    var timeout = (TFly.status=="active") ? 0 : TFly.popupLatency
    if(TFly.AnyIter>0 && TFly.AnyTime>0)
      TFly.timer = window.setTimeout(function(){TFly_AnimateStart(TFly,true,TFly.F.curIter);},timeout)
    else
      TFly.timer = window.setTimeout(function(){TFly_SetVisibility(TFly,true);},timeout)
  }else{
    if(TFly.status=="off") return
    var timeout = (TFly.status=="active") ? 0 : TFly.hideLatency
    if(TFly.AnyIter>0 && TFly.AnyTime>0)
      TFly.timer = window.setTimeout(function(){TFly_AnimateStart(TFly,false,TFly.F.curIter);},timeout)
    else
      TFly.timer = window.setTimeout(function(){TFly_SetVisibility(TFly,false);},timeout)
  }
}

function TFly_SetVisibility(TFly, value){
    if (value == true) {
        TFly_SetPosition(TFly, TFly.DT);        
        TFly.style.zIndex = "199";
        TFly_Reposition(TFly);
        TFly.S.style.position = "absolute";
        TFly.S.style.position = "";
        TFly.DT.style.visibility = "visible";
    } else {

        TFly.DT.style.visibility = "hidden";
        TFly.style.zIndex = "198";
        TFly.S.style.position = "absolute";
        TFly.S.style.position = "";
    }
}

function TFly_AnimateStart(TFly, inout, curIter){
    if(TFly.F.timer!=null) window.clearTimeout(TFly.F.timer);
    if(curIter==null) curIter=0;
    if(TFly.status !="active"){
        TFly.status ="active";
        TFly.F.style.visibility = "visible";
        TFly.DT.style.visibility="hidden";
    }
    if(curIter<0 || curIter>TFly.AnyIter ){
        TFly.F.curIter = (curIter<1) ? 0 : TFly.AnyIter;
        TFly.status = (TFly.F.curIter==TFly.AnyIter) ? "on": "off";
        TFly.F.style.visibility = "hidden";
        TFly_SetPosition(TFly, TFly.DT);

        TFly.style.zIndex = (TFly.F.curIter == TFly.AnyIter) ? "999" : "0";
        if(TFly.F.curIter==TFly.AnyIter){
          TFly.status = "on";
          TFly_SetVisibility(TFly,true);
          TFly_CallClientFunction(TFly.onPopup, TFly)
        }else{
          TFly.status = "off";
          TFly_SetVisibility(TFly,false);
          TFly_CallClientFunction(TFly.onHide, TFly)
        }
    }else{
        var per = curIter*(95/TFly.AnyIter)+ 5;
        TFly.F.style.visibility = "hidden";
        TFly.F.style.width=TFly.DT.offsetWidth/100*per + "px";
        TFly.F.style.height=TFly.DT.offsetHeight/100*per + "px";
        TFly_SetPosition(TFly, TFly.F);
        TFly.F.style.visibility = "visible";
        var timeout = (per==100 ) ? 0 : TFly.AnyTime - (TFly.AnyTime/(TFly.AnyIter)*(curIter)); 
        curIter += (inout) ? 1:-1;
        TFly.F.timer = window.setTimeout(function(){TFly_AnimateStart(TFly, inout, curIter)},timeout);
        TFly.F.curIter = curIter;
    }
}

