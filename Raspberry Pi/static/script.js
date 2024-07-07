document.addEventListener("DOMContentLoaded", function() {
    const modeButtons = document.querySelectorAll(".mode");
    const directionButtons = document.querySelectorAll(".direction-controls button");
    const stopButton = document.getElementById("stop");
    const manualButton = document.getElementById("manual");
    const autoButton = document.getElementById("automatic");
    const checkText = document.getElementById("check");
  
    let activeMode = null;
    let activeDirectionButton = null;
  
    modeButtons.forEach(button => {
        button.addEventListener("click", function() {
            modeButtons.forEach(btn => btn.classList.remove("active"));
            this.classList.add("active");
            activeMode = this.id;
            if (activeMode === "automatic") {
                directionButtons.forEach(button => {
                    button.disabled = true;
                    autoButton.disabled = true;
                    manualButton.disabled = false;
                    button.style.backgroundColor = ""; // Reset background color
                });
            } else {
                directionButtons.forEach(button => {
                    button.disabled = false;
                });
                directionButtons.forEach(btn => btn.style.backgroundColor = ""); // Reset background color
                stopButton.click(); // Simulate a click on the stop button
            }
            // Send the button action to the server
            sendButtonAction(this.id);
        });
    });
  
    directionButtons.forEach(button => {
        button.addEventListener("click", function() {
            if (activeMode === "manual") {
                manualButton.disabled = true;
                autoButton.disabled = false;
                if (activeDirectionButton && activeDirectionButton !== this) {
                    activeDirectionButton.style.backgroundColor = "";
                }
                activeDirectionButton = this;
                this.style.backgroundColor = "orange";
                // Send the button action to the server
                sendButtonAction(this.id);
            }
        });
    });
  
    // Function to send button action to the server
    function sendButtonAction(action) {
        fetch(`/button_action`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({ action: action })
        });
    }
  
    // Function to get max value from the server and show/hide checkText based on its value
    function getMaxValue() {
      fetch('/get_max_value')
          .then(response => response.json())
          .then(data => {
              let max = data.max;
              if (max > 28) {
                  checkText.style.display = "block";
              } else {
                  checkText.style.display = "none";
              }
          })
          .catch(error => console.error('Error fetching max value:', error));
    }
  
      // Automatically check the value every 1 second
      setInterval(getMaxValue, 1000);
});
  