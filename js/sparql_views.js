// $Id: 
(function ($) {

  Drupal.behaviors.prefixSelection = function (context) {
      $('.prefix-store').each(function(index) {
        wrapper = $(this).parents('div.form-item.form-type-textarea');
        wrapper.data('changed',false);
        $(this).parents('.resizable-textarea').hide();
        $(this).hide();
        $(this).parent().parent().after(Drupal.theme('rdfPrefixWidget', wrapper, index));
        initPrefixes(wrapper);
      });
      
      $('.prefix-add').click(function(){
        wrapper = $(this).parents('div.form-item.form-type-textarea');
        // Get the value from the text field.
        val = $(this).prev().val();
        // Test whether the prefix is already added to this field. If it is,
        // fall out of if statement and do nothing.
        if ($(wrapper).find('.prefix-val').filter(function(){
          return $(this).text() == val;
        }).length == 0) {
          // Ensure that this value is formatted as namespace:prefix based on the
          // regular expression defined in rdfui.module.
          // @todo Check that the namespace is a valid namespace.
          pattern = eval(Drupal.settings.rdfui.prefixRegex);
          if (pattern.test(val)) {
            // Theme the prefix and add it to the prefix holder above the
            // field.
            $(wrapper).find('.prefix-holder').append(Drupal.theme('rdfPrefix',val, false));
            // Make the 'x' a button that removes the prefix.
            bindRemoveClicks(wrapper);
            // Update the hidden textarea that will be used to submit the
            // prefixes added by the field.
            updateStore(wrapper);
            // Empty the field so user can add another prefix.
            $(this).prev().val('');        
          }
          // If the value didn't pass the regular expression, alert the user.
          else {
            alert(Drupal.t(val + ' is not formatted correctly.'));
          }
        }
        
        // Return false so the form does not submit.
        return false;
      });
      
      function initPrefixes(wrapper) {
        textarea = wrapper.find('.prefix-store');
        wrapper.find('.prefix-holder').children().remove();
        jQuery.each($(textarea).html().split('\n'), function(i,val) {
          if (jQuery.trim(val) != '') {
            $(wrapper).find('.prefix-holder').append(Drupal.theme('rdfPrefix',val, true));
          }
        });
        bindRemoveClicks(wrapper);
      }
      
      function updateStore(wrapper) {
        // Refresh the values in the hidden textarea that is used to submit the
        // form.
        textarea = wrapper.find('.prefix-store');
        textarea.html('');
        wrapper.find('.prefix-val').each(function(){
          textarea.append($(this).html() + '\n');
        });
        
        // If a new prefix is added, display a message that reminds the user
        // to save changes.
        if(wrapper.data('changed') == false) {
          $(wrapper).find('.prefixes-widget').prepend(Drupal.theme('rdfPrefixesChangedWarning')).hide().fadeIn();
          wrapper.data('changed',true);
        }
      }
      
      function bindRemoveClicks(wrapper) {
        $(wrapper).find('.prefix-remove:not(.prefix-process)').click(function(){
          $(this).parent().remove();
          updateStore(wrapper);          
        }).addClass('.prefix-process');

      }
            
    };
  
  Drupal.theme.prototype.rdfPrefix = function(val, saved) {
    saved_class = '';
    saved_text = '';
    if (!saved) {
      saved_class = ' prefix-unsaved';
      saved_text = '<span class="warning">*</span>';
    }
    return '<span class="prefix'+ saved_class +'"><span class="prefix-val">' + val + '</span>'+
    saved_text +' <span class="prefix-remove">x</span></span>';
  }
  
  Drupal.theme.prototype.rdfPrefixWidget = function(context, index) {
    var wrapper = $(context);
    var basepath = Drupal.settings.basePath;
    var description = jQuery('.prefix-store + .description').eq(0);
    var html_output ='<div id="" class="form-item prefixes-widget">' +
      '<div class="prefix-holder"></div>' +
      '<input type="text" class="prefix-entry form-autocomplete" size="30" id="rdfui-prefix-edit-'+ index +'" />' +
      '<input type="submit" value="' + Drupal.t('Add') + '" class="form-submit prefix-add" id="rdfui-prefix-edit-button-'+ index +'">' +
      '<input class="autocomplete" type="hidden" id="rdfui-prefix-edit-'+ index +'-autocomplete" ' +
      'value="' + basepath + '/rdfui/prefixes/autocomplete" disabled="disabled" />' +
      '<div class="description">' + description.text() + '</div>' +
    '</div>';
    description.remove();
    return html_output;
  };
  
  Drupal.theme.prototype.rdfPrefixesChangedWarning = function() {
    return '<div class="warning">* ' + Drupal.t('Changes made will not be saved until the form is submitted.') + '</div>'  
  }
 

})(jQuery);
