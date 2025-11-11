// Ride Suggestions
async function showRideSuggestions() {
    const visitorId = prompt('Enter Visitor ID to get ride suggestions:');
    if (!visitorId || parseInt(visitorId) < 1000) {
        showNotification('Invalid visitor ID', 'error');
        return;
    }
    
    try {
        const response = await fetch(`${API_BASE}/visitors/${visitorId}/suggest`);
        const data = await response.json();
        
        if (response.ok) {
            const modal = document.createElement('div');
            modal.className = 'modal';
            modal.innerHTML = `
                <div class="modal-content">
                    <span class="close" onclick="this.parentElement.parentElement.remove()">&times;</span>
                    <h2>🎯 Recommended Rides</h2>
                    <div class="suggestions-list">
                        ${data.suggestions.map(ride => `
                            <div class="suggestion-card">
                                <h3>${ride.name}</h3>
                                <p>⭐ Thrill Match: ${Math.round(ride.thrill_match)}/10</p>
                                <p>⏱️ Wait Time: ${ride.wait_time} min</p>
                                <p>📊 Priority Score: ${Math.round(ride.priority_score)}</p>
                                <button onclick="enjoyRide(${ride.ride_id})">🎢 Enjoy This Ride</button>
                            </div>
                        `).join('')}
                    </div>
                </div>
            `;
            document.body.appendChild(modal);
        } else {
            showNotification(data.error || 'Failed to get suggestions', 'error');
        }
    } catch (error) {
        console.error('Error getting suggestions:', error);
        showNotification('Error getting ride suggestions', 'error');
    }
}

// Visitor History
async function showVisitorHistory() {
    const visitorId = prompt('Enter Visitor ID to view ride history:');
    if (!visitorId || parseInt(visitorId) < 1000) {
        showNotification('Invalid visitor ID', 'error');
        return;
    }
    
    try {
        const response = await fetch(`${API_BASE}/visitors/${visitorId}/history`);
        const data = await response.json();
        
        if (response.ok) {
            const modal = document.createElement('div');
            modal.className = 'modal';
            modal.innerHTML = `
                <div class="modal-content">
                    <span class="close" onclick="this.parentElement.parentElement.remove()">&times;</span>
                    <h2>📜 Ride History</h2>
                    <div class="history-list">
                        ${data.rides.length ? data.rides.map((ride, index) => `
                            <div class="history-item">
                                <span class="history-number">${index + 1}</span>
                                <span class="ride-name">${ride.name}</span>
                                <span class="ride-time">${new Date(ride.timestamp * 1000).toLocaleTimeString()}</span>
                                ${index === 0 ? `<button onclick="undoLastRide(${visitorId})">↩️ Undo</button>` : ''}
                            </div>
                        `).join('') : '<p>No rides yet</p>'}
                    </div>
                </div>
            `;
            document.body.appendChild(modal);
        } else {
            showNotification(data.error || 'Failed to get history', 'error');
        }
    } catch (error) {
        console.error('Error getting history:', error);
        showNotification('Error getting ride history', 'error');
    }
}

// Wait Times (BST)
async function showWaitTimes() {
    try {
        const response = await fetch(`${API_BASE}/rides/by-wait-time`);
        const data = await response.json();
        
        if (response.ok) {
            const view = document.querySelector('.view-section.active');
            view.innerHTML = '';
            
            const waitTimeView = document.createElement('div');
            waitTimeView.className = 'wait-time-view';
            waitTimeView.innerHTML = `
                <h3>⏱️ Rides Sorted by Wait Time</h3>
                <div class="wait-time-list">
                    ${data.rides.map(ride => `
                        <div class="wait-time-item">
                            <span class="ride-name">${ride.name}</span>
                            <span class="wait-badge">${ride.wait_time} min</span>
                        </div>
                    `).join('')}
                </div>
            `;
            view.appendChild(waitTimeView);
        } else {
            showNotification(data.error || 'Failed to get wait times', 'error');
        }
    } catch (error) {
        console.error('Error getting wait times:', error);
        showNotification('Error getting wait times', 'error');
    }
}

// Path Finding
async function showPathFinder() {
    const modal = document.createElement('div');
    modal.className = 'modal';
    modal.innerHTML = `
        <div class="modal-content">
            <span class="close" onclick="this.parentElement.parentElement.remove()">&times;</span>
            <h2>🗺️ Find Path Between Rides</h2>
            <div class="pathfinder-form">
                <div class="form-group">
                    <label>From Ride:</label>
                    <select id="from-ride"></select>
                </div>
                <div class="form-group">
                    <label>To Ride:</label>
                    <select id="to-ride"></select>
                </div>
                <button onclick="findPath()">🔍 Find Path</button>
            </div>
            <div id="path-result"></div>
        </div>
    `;
    document.body.appendChild(modal);
    
    // Populate ride dropdowns
    const rides = await (await fetch(`${API_BASE}/rides`)).json();
    const fromSelect = document.getElementById('from-ride');
    const toSelect = document.getElementById('to-ride');
    
    rides.forEach(ride => {
        fromSelect.innerHTML += `<option value="${ride.id}">${ride.name}</option>`;
        toSelect.innerHTML += `<option value="${ride.id}">${ride.name}</option>`;
    });
}

async function findPath() {
    const fromRide = document.getElementById('from-ride').value;
    const toRide = document.getElementById('to-ride').value;
    
    try {
        const response = await fetch(`${API_BASE}/pathfind`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ from_ride: parseInt(fromRide), to_ride: parseInt(toRide) })
        });
        
        const data = await response.json();
        if (response.ok) {
            const result = document.getElementById('path-result');
            result.innerHTML = `
                <h3>📍 Recommended Path</h3>
                <div class="path-steps">
                    ${data.path.map((step, index) => `
                        <div class="path-step">
                            <span class="step-number">${index + 1}</span>
                            <span class="ride-name">${step.name}</span>
                            <span class="distance">${step.distance}m</span>
                            ${index < data.path.length - 1 ? '<span class="arrow">➡️</span>' : ''}
                        </div>
                    `).join('')}
                </div>
                <p class="total-distance">🚶 Total Distance: ${data.path.reduce((sum, step) => sum + step.distance, 0)}m</p>
            `;
        } else {
            showNotification(data.error || 'Failed to find path', 'error');
        }
    } catch (error) {
        console.error('Error finding path:', error);
        showNotification('Error finding path', 'error');
    }
}

// Undo Last Ride
async function undoLastRide(visitorId) {
    if (!confirm('Are you sure you want to undo the last ride?')) return;
    
    try {
        const response = await fetch(`${API_BASE}/visitors/${visitorId}/undo`, {
            method: 'POST'
        });
        
        const data = await response.json();
        if (response.ok) {
            showNotification(`Undid last ride: ${data.ride.name}`, 'success');
            showVisitorHistory(); // Refresh history
        } else {
            showNotification(data.error || 'Failed to undo ride', 'error');
        }
    } catch (error) {
        console.error('Error undoing ride:', error);
        showNotification('Error undoing ride', 'error');
    }
}

// View Toggle
function toggleView(view) {
    const buttons = document.querySelectorAll('.view-btn');
    buttons.forEach(btn => btn.classList.remove('active'));
    document.querySelector(`[onclick="toggleView('${view}')"]`).classList.add('active');
    
    // Get or create the view section
    let viewSection = document.querySelector('#rides-grid');
    if (!viewSection) {
        viewSection = document.createElement('div');
        viewSection.id = 'rides-grid';
        viewSection.className = 'view-section active';
        document.querySelector('#rides').appendChild(viewSection);
    }
    
    // Clear and reset classes
    viewSection.className = 'view-section active';
    
    switch(view) {
        case 'grid':
            viewSection.className = 'rides-grid view-section active';
            loadRides();
            break;
        case 'wait-time':
            viewSection.className = 'view-section active';
            showWaitTimes();
            break;
        case 'path':
            viewSection.className = 'view-section active';
            showPathFinder();
            break;
    }
}