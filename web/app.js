// API Base URL
const API_BASE = 'http://localhost:8000/api';

// State
let visitors = [];
let rides = [];
let queues = [];

// Initialize
document.addEventListener('DOMContentLoaded', () => {
    setupNavigation();
    setupForms();
    loadDashboard();
    
    // Auto-refresh every 5 seconds
    setInterval(() => {
        if (document.querySelector('.section.active').id === 'dashboard') {
            loadDashboard();
        }
    }, 5000);
});

// Navigation
function setupNavigation() {
    const navBtns = document.querySelectorAll('.nav-btn');
    navBtns.forEach(btn => {
        btn.addEventListener('click', () => {
            // Remove active class from all
            navBtns.forEach(b => b.classList.remove('active'));
            document.querySelectorAll('.section').forEach(s => s.classList.remove('active'));
            
            // Add active to clicked
            btn.classList.add('active');
            const sectionId = btn.getAttribute('data-section');
            document.getElementById(sectionId).classList.add('active');
            
            // Load section data
            loadSectionData(sectionId);
        });
    });
}

// Load section data
function loadSectionData(section) {
    switch(section) {
        case 'dashboard':
            loadDashboard();
            break;
        case 'visitors':
            loadVisitors();
            break;
        case 'rides':
            loadRides();
            break;
        case 'queues':
            loadQueues();
            break;
        case 'stats':
            loadStatistics();
            break;
    }
}

// Dashboard
async function loadDashboard() {
    try {
        const [visitorsRes, ridesRes, statsRes] = await Promise.all([
            fetch(`${API_BASE}/visitors`),
            fetch(`${API_BASE}/rides`),
            fetch(`${API_BASE}/stats`)
        ]);
        
        visitors = await visitorsRes.json();
        rides = await ridesRes.json();
        const stats = await statsRes.json();
        
        // Update dashboard stats
        document.getElementById('total-visitors').textContent = visitors.length;
        document.getElementById('total-rides').textContent = rides.filter(r => r.is_operational).length;
        document.getElementById('avg-wait').textContent = `${stats.avg_wait_time || 0} min`;
        document.getElementById('satisfaction').textContent = `${Math.round(stats.avg_satisfaction || 0)}%`;
        document.getElementById('visitor-count').textContent = `Visitors: ${visitors.length}`;
    } catch (error) {
        console.error('Error loading dashboard:', error);
        showNotification('Failed to load dashboard data', 'error');
    }
}

// Visitors
async function loadVisitors() {
    try {
        const response = await fetch(`${API_BASE}/visitors`);
        visitors = await response.json();
        
        const grid = document.getElementById('visitors-grid');
        grid.innerHTML = '';
        
        visitors.forEach(visitor => {
            const card = createVisitorCard(visitor);
            grid.appendChild(card);
        });
        
        // Setup search
        document.getElementById('visitor-search').addEventListener('input', (e) => {
            filterVisitors(e.target.value);
        });
    } catch (error) {
        console.error('Error loading visitors:', error);
        showNotification('Failed to load visitors', 'error');
    }
}

function createVisitorCard(visitor) {
    const card = document.createElement('div');
    card.className = 'visitor-card';
    
    const ticketClass = visitor.ticket_type === 1 ? 'ticket-premium' : 'ticket-normal';
    const ticketText = visitor.ticket_type === 1 ? '⭐ Premium' : '🎫 Normal';
    
    card.innerHTML = `
        <h3>👤 ${visitor.name}</h3>
        <div class="visitor-info">
            <span><strong>ID:</strong> ${visitor.id}</span>
            <span><strong>Ticket:</strong> <span class="ticket-badge ${ticketClass}">${ticketText}</span></span>
            <span><strong>Thrill Level:</strong> ${visitor.thrill_preference}/10 ${'🔥'.repeat(Math.ceil(visitor.thrill_preference / 3))}</span>
            <span><strong>Rides Done:</strong> ${visitor.rides_completed} 🎢</span>
            <span><strong>Satisfaction:</strong> ${Math.round(visitor.satisfaction_score)}% ${getSatisfactionEmoji(visitor.satisfaction_score)}</span>
            <span><strong>Distance:</strong> ${visitor.total_distance_traveled}m 🚶</span>
        </div>
        <button class="btn-delete" onclick="deleteVisitor(${visitor.id})">🗑️ Remove</button>
    `;
    
    return card;
}

function getSatisfactionEmoji(score) {
    if (score >= 80) return '😄';
    if (score >= 60) return '🙂';
    if (score >= 40) return '😐';
    return '😞';
}

function filterVisitors(query) {
    const cards = document.querySelectorAll('.visitor-card');
    cards.forEach(card => {
        const text = card.textContent.toLowerCase();
        card.style.display = text.includes(query.toLowerCase()) ? 'block' : 'none';
    });
}

// Rides
async function loadRides() {
    try {
        const response = await fetch(`${API_BASE}/rides`);
        rides = await response.json();
        
        const grid = document.getElementById('rides-grid');
        grid.innerHTML = '';
        
        rides.forEach(ride => {
            const card = createRideCard(ride);
            grid.appendChild(card);
        });
    } catch (error) {
        console.error('Error loading rides:', error);
        showNotification('Failed to load rides', 'error');
    }
}

function createRideCard(ride) {
    const card = document.createElement('div');
    card.className = 'ride-card';
    
    const statusClass = ride.is_operational ? 'status-active' : 'status-inactive';
    const statusText = ride.is_operational ? '✅ Open' : '🔴 Closed';
    
    // Map ride to image
    const rideImage = getRideImage(ride.name, ride.thrill_level);
    
    card.innerHTML = `
        <img src="${rideImage}" alt="${ride.name}" class="ride-image" onerror="this.src='images/default-ride.jpg'">
        <div class="ride-content">
            <h3>🎢 ${ride.name}</h3>
            <div class="ride-details">
                <span><strong>Thrill Level:</strong> ${ride.thrill_level}/10 ${'⚡'.repeat(Math.ceil(ride.thrill_level / 3))}</span>
                <span><strong>Capacity:</strong> ${ride.capacity} people</span>
                <span><strong>Wait Time:</strong> ${ride.wait_time || 0} min ⏳</span>
                <span><strong>Queue:</strong> ${ride.queue_size || 0} people 👥</span>
            </div>
            <span class="ride-status ${statusClass}">${statusText}</span>
            <div class="ride-actions">
                <button class="btn-enjoy" onclick="enjoyRide(${ride.id})" ${!ride.is_operational ? 'disabled' : ''}>
                    🎢 Enjoy Ride
                </button>
                <button class="btn-toggle" onclick="toggleRideStatus(${ride.id})">
                    ${ride.is_operational ? '🔒 Close for Maintenance' : '✅ Open Ride'}
                </button>
                <button class="btn-delete" onclick="deleteRide(${ride.id})">🗑️ Delete</button>
            </div>
        </div>
    `;
    
    return card;
}

function getRideImage(name, thrillLevel) {
    // Map ride names to specific images
    const nameLower = name.toLowerCase();
    
    // Check for specific ride type keywords
    if (nameLower.includes('roller') || nameLower.includes('coaster')) {
        return 'images/rollercoaster.jpg';
    }
    if (nameLower.includes('wheel') || nameLower.includes('ferris')) {
        return 'images/ferris-wheel.jpg';
    }
    if (nameLower.includes('carousel') || nameLower.includes('merry')) {
        return 'images/carousel.jpg';
    }
    if (nameLower.includes('drop') || nameLower.includes('tower') || nameLower.includes('fall')) {
        return 'images/drop-tower.jpg';
    }
    if (nameLower.includes('swing') || nameLower.includes('pirate') || nameLower.includes('ship')) {
        return 'images/swing.jpg';
    }
    
    // Fallback to thrill-level based images
    if (thrillLevel >= 8) {
        return 'images/extreme-ride.jpg';
    } else if (thrillLevel >= 5) {
        return 'images/medium-ride.jpg';
    } else if (thrillLevel >= 1) {
        return 'images/family-ride.jpg';
    }
    
    // Final fallback to default
    return 'images/default-ride.jpg';
}

// Queues
async function loadQueues() {
    try {
        const response = await fetch(`${API_BASE}/queues`);
        queues = await response.json();
        
        const container = document.getElementById('queues-container');
        container.innerHTML = '';
        
        queues.forEach(queue => {
            const item = createQueueItem(queue);
            container.appendChild(item);
        });
    } catch (error) {
        console.error('Error loading queues:', error);
        showNotification('Failed to load queues', 'error');
    }
}

function createQueueItem(queue) {
    const item = document.createElement('div');
    item.className = 'queue-item';
    
    const percentage = Math.min((queue.size / queue.capacity) * 100, 100);
    const statusClass = queue.is_operational ? 'operational' : 'maintenance';
    const queueStatus = queue.is_operational ? '🟢 Operating' : '🔧 Maintenance';
    
    item.innerHTML = `
        <h3>🎢 ${queue.ride_name}</h3>
        <div class="ride-status ${statusClass}">${queueStatus}</div>
        <div class="ride-details">
            <span><strong>Regular Queue:</strong> ${queue.size - (queue.premium_size || 0)} people</span>
            <span><strong>Fast-Pass Queue:</strong> ${queue.premium_size || 0} people</span>
            <span><strong>Est. Wait Time:</strong> ${queue.wait_time} min</span>
            <span><strong>Capacity:</strong> ${queue.capacity} per ride</span>
        </div>
        <div class="queue-bar">
            <div class="queue-fill" style="width: ${percentage}%">
                ${Math.round(percentage)}% Full
            </div>
        </div>
    `;
    
    return item;
}

// Statistics
async function loadStatistics() {
    try {
        const response = await fetch(`${API_BASE}/stats`);
        let stats = {};
        try {
            stats = await response.json();
        } catch (e) {
            stats = {};
        }
        // Provide defaults if missing
        const total_visitors = stats.total_visitors ?? 0;
        const premium_visitors = stats.premium_visitors ?? 0;
        const avg_satisfaction = Math.round(stats.avg_satisfaction ?? 0);
        const total_distance = stats.total_distance ?? 0;
        
        // Visitor stats
        const visitorStats = document.getElementById('visitor-stats');
        visitorStats.innerHTML = `
            <p><strong>Total Visitors:</strong> ${total_visitors}</p>
            <p><strong>Premium Visitors:</strong> ${premium_visitors}</p>
            <p><strong>Avg Satisfaction:</strong> ${avg_satisfaction}%</p>
            <p><strong>Total Distance:</strong> ${total_distance}m</p>
        `;
        
        // Ride stats
        const rideStats = document.getElementById('ride-stats');
        rideStats.innerHTML = `
            <p><strong>Total Rides:</strong> ${stats.total_rides ?? 0}</p>
            <p><strong>Active Rides:</strong> ${stats.active_rides ?? 0}</p>
            <p><strong>Avg Wait Time:</strong> ${stats.avg_wait_time ?? 0} min</p>
            <p><strong>Total Capacity:</strong> ${stats.total_capacity ?? 0} people</p>
        `;
        
        // Popular rides
        const popularRides = document.getElementById('popular-rides');
        if (stats.popular_rides && stats.popular_rides.length > 0) {
            popularRides.innerHTML = stats.popular_rides.map((ride, idx) => 
                `<p>${idx + 1}. <strong>${ride.name}</strong> - ${ride.visitors} visitors</p>`
            ).join('');
        } else {
            popularRides.innerHTML = '<p>No data available</p>';
        }
    } catch (error) {
        // Always show at least the visitor analytics box with 0s
        document.getElementById('visitor-stats').innerHTML = `
            <p><strong>Total Visitors:</strong> 0</p>
            <p><strong>Premium Visitors:</strong> 0</p>
            <p><strong>Avg Satisfaction:</strong> 0%</p>
            <p><strong>Total Distance:</strong> 0m</p>
        `;
        document.getElementById('ride-stats').innerHTML = '';
        document.getElementById('popular-rides').innerHTML = '';
        showNotification('Failed to load statistics', 'error');
    }
}

// Forms
function setupForms() {
    // Add Visitor Form
    document.getElementById('add-visitor-form').addEventListener('submit', async (e) => {
        e.preventDefault();
        
        const visitorData = {
            name: document.getElementById('visitor-name').value,
            thrill_preference: parseInt(document.getElementById('thrill-preference').value),
            ticket_type: parseInt(document.getElementById('ticket-type').value)
        };
        
        try {
            const response = await fetch(`${API_BASE}/visitors`, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify(visitorData)
            });
            
            if (response.ok) {
                showNotification('Visitor added successfully!', 'success');
                closeModal('add-visitor-modal');
                e.target.reset();
                loadVisitors();
                loadDashboard();
            } else {
                showNotification('Failed to add visitor', 'error');
            }
        } catch (error) {
            console.error('Error adding visitor:', error);
            showNotification('Error adding visitor', 'error');
        }
    });
    
    // Add Ride Form
    document.getElementById('add-ride-form').addEventListener('submit', async (e) => {
        e.preventDefault();
        
        const rideData = {
            name: document.getElementById('ride-name').value,
            thrill_level: parseInt(document.getElementById('ride-thrill').value),
            capacity: parseInt(document.getElementById('ride-capacity').value),
            duration: parseInt(document.getElementById('ride-duration').value)
        };
        
        try {
            const response = await fetch(`${API_BASE}/rides`, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify(rideData)
            });
            
            if (response.ok) {
                showNotification('Ride added successfully!', 'success');
                closeModal('add-ride-modal');
                e.target.reset();
                loadRides();
                loadDashboard();
            } else {
                showNotification('Failed to add ride', 'error');
            }
        } catch (error) {
            console.error('Error adding ride:', error);
            showNotification('Error adding ride', 'error');
        }
    });
}

// Modal functions
function showAddVisitor() {
    document.getElementById('add-visitor-modal').style.display = 'block';
}

function showAddRide() {
    document.getElementById('add-ride-modal').style.display = 'block';
}

function closeModal(modalId) {
    document.getElementById(modalId).style.display = 'none';
}

// Close modal when clicking outside
window.onclick = function(event) {
    if (event.target.classList.contains('modal')) {
        event.target.style.display = 'none';
    }
}

// Refresh data
function refreshData() {
    loadDashboard();
    showNotification('Data refreshed!', 'success');
}

// Notification system
function showNotification(message, type = 'info') {
    const notification = document.createElement('div');
    notification.style.cssText = `
        position: fixed;
        top: 100px;
        right: 20px;
        padding: 1rem 2rem;
        background: ${type === 'success' ? '#4ade80' : type === 'error' ? '#f87171' : '#60a5fa'};
        color: white;
        border-radius: 10px;
        box-shadow: 0 5px 20px rgba(0,0,0,0.2);
        z-index: 10000;
        animation: slideIn 0.3s ease;
    `;
    notification.textContent = message;
    
    document.body.appendChild(notification);
    
    setTimeout(() => {
        notification.style.animation = 'slideOut 0.3s ease';
        setTimeout(() => notification.remove(), 300);
    }, 3000);
}

// Toggle ride status
async function toggleRideStatus(rideId) {
    try {
        const response = await fetch(`${API_BASE}/rides/${rideId}/toggle`, {
            method: 'PUT'
        });
        
        if (response.ok) {
            const data = await response.json();
            const statusMsg = data.is_operational ? 'Ride opened!' : 'Ride closed for maintenance!';
            showNotification(statusMsg, 'success');
            loadRides();
            loadDashboard();
            loadQueues();
        } else {
            showNotification('Failed to update ride status', 'error');
        }
    } catch (error) {
        console.error('Error toggling ride:', error);
        showNotification('Error updating ride status', 'error');
    }
}

// Delete ride
async function deleteRide(rideId) {
    if (!confirm('Are you sure you want to delete this ride?')) return;
    
    try {
        const response = await fetch(`${API_BASE}/rides/${rideId}`, {
            method: 'DELETE'
        });
        
        if (response.ok) {
            showNotification('Ride deleted successfully!', 'success');
            loadRides();
            loadDashboard();
        } else {
            showNotification('Failed to delete ride', 'error');
        }
    } catch (error) {
        console.error('Error deleting ride:', error);
        showNotification('Error deleting ride', 'error');
    }
}

// Delete visitor
async function deleteVisitor(visitorId) {
    if (!confirm('Are you sure you want to remove this visitor?')) return;
    
    try {
        const response = await fetch(`${API_BASE}/visitors/${visitorId}`, {
            method: 'DELETE'
        });
        
        if (response.ok) {
            showNotification('Visitor removed successfully!', 'success');
            loadVisitors();
            loadDashboard();
        } else {
            showNotification('Failed to remove visitor', 'error');
        }
    } catch (error) {
        console.error('Error removing visitor:', error);
        showNotification('Error removing visitor', 'error');
    }
}

// Enjoy ride with timer
async function enjoyRide(rideId) {
    const visitorId = prompt('Enter your Visitor ID (must be 1000 or higher):');
    if (!visitorId || parseInt(visitorId) < 1000) {
        showNotification('Invalid visitor ID. Must be 1000 or higher.', 'error');
        return;
    }
    
    // Get ride details first
    const ride = rides.find(r => r.id === rideId);
    if (!ride) {
        showNotification('Ride not found', 'error');
        return;
    }
    
    // Calculate ride duration based on thrill level (same as backend)
    const rideDuration = 30 + (ride.thrill_level * 10); // 40-130 seconds
    
    // Show timer modal
    showRideTimerModal(ride.name, rideDuration);
    
    try {
        // Start countdown
        await startRideCountdown(rideDuration);
        
        // After countdown, call the API
        const response = await fetch(`${API_BASE}/rides/${rideId}/experience`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ visitor_id: parseInt(visitorId) })
        });
        
        if (response.ok) {
            const data = await response.json();
            closeRideTimerModal();
            showNotification(`🎉 ${data.message} ${data.visitor_name}'s Satisfaction: ${Math.round(data.satisfaction)}%`, 'success');
            loadVisitors();
            loadDashboard();
            loadRides();
            loadVisitors();
            loadQueues();
            loadDashboard();
        } else {
            const error = await response.json();
            showNotification(error.error || 'Failed to enjoy ride', 'error');
        }
    } catch (error) {
        console.error('Error enjoying ride:', error);
        closeRideTimerModal();
        showNotification('Error enjoying ride', 'error');
    }
}

// Show ride timer modal
function showRideTimerModal(rideName, duration) {
    const modal = document.createElement('div');
    modal.id = 'ride-timer-modal';
    modal.style.cssText = `
        position: fixed;
        top: 0;
        left: 0;
        width: 100%;
        height: 100%;
        background: rgba(0,0,0,0.8);
        display: flex;
        justify-content: center;
        align-items: center;
        z-index: 10000;
    `;
    
    modal.innerHTML = `
        <div style="background: white; padding: 3rem; border-radius: 20px; text-align: center; min-width: 400px; box-shadow: 0 10px 40px rgba(0,0,0,0.3);">
            <h2 style="color: #667eea; margin-bottom: 1rem;">🎢 Enjoying ${rideName}!</h2>
            <div style="font-size: 4rem; font-weight: bold; color: #764ba2; margin: 2rem 0;" id="timer-display">
                ${Math.floor(duration / 60)}:${(duration % 60).toString().padStart(2, '0')}
            </div>
            <div style="background: #f0f0f0; height: 20px; border-radius: 10px; overflow: hidden; margin: 1rem 0;">
                <div id="timer-progress" style="background: linear-gradient(90deg, #667eea 0%, #764ba2 100%); height: 100%; width: 100%; transition: width 1s linear;"></div>
            </div>
            <p style="color: #666; font-size: 1.1rem;">⏳ Please wait while the ride completes...</p>
        </div>
    `;
    
    document.body.appendChild(modal);
}

// Start ride countdown
function startRideCountdown(duration) {
    return new Promise((resolve) => {
        let timeLeft = duration;
        const timerDisplay = document.getElementById('timer-display');
        const progressBar = document.getElementById('timer-progress');
        
        const interval = setInterval(() => {
            timeLeft--;
            
            // Update timer display
            const minutes = Math.floor(timeLeft / 60);
            const seconds = timeLeft % 60;
            timerDisplay.textContent = `${minutes}:${seconds.toString().padStart(2, '0')}`;
            
            // Update progress bar
            const progress = ((duration - timeLeft) / duration) * 100;
            progressBar.style.width = `${100 - progress}%`;
            
            if (timeLeft <= 0) {
                clearInterval(interval);
                resolve();
            }
        }, 1000);
    });
}

// Close ride timer modal
function closeRideTimerModal() {
    const modal = document.getElementById('ride-timer-modal');
    if (modal) {
        modal.remove();
    }
}

// Add animations
const style = document.createElement('style');
style.textContent = `
    @keyframes slideIn {
        from { transform: translateX(400px); opacity: 0; }
        to { transform: translateX(0); opacity: 1; }
    }
    @keyframes slideOut {
        from { transform: translateX(0); opacity: 1; }
        to { transform: translateX(400px); opacity: 0; }
    }
    .btn-toggle, .btn-delete, .btn-enjoy {
        padding: 0.5rem 1rem;
        margin: 0.25rem;
        border: none;
        border-radius: 5px;
        cursor: pointer;
        font-size: 0.9rem;
        transition: all 0.3s;
    }
    .btn-enjoy {
        background: linear-gradient(135deg, #11998e 0%, #38ef7d 100%);
        color: white;
        font-weight: bold;
    }
    .btn-enjoy:hover:not(:disabled) {
        transform: translateY(-2px);
        box-shadow: 0 5px 15px rgba(56, 239, 125, 0.4);
    }
    .btn-enjoy:disabled {
        opacity: 0.5;
        cursor: not-allowed;
    }
    .btn-toggle {
        background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
        color: white;
    }
    .btn-toggle:hover {
        transform: translateY(-2px);
        box-shadow: 0 5px 15px rgba(102, 126, 234, 0.4);
    }
    .btn-delete {
        background: linear-gradient(135deg, #f093fb 0%, #f5576c 100%);
        color: white;
    }
    .btn-delete:hover {
        transform: translateY(-2px);
        box-shadow: 0 5px 15px rgba(245, 87, 108, 0.4);
    }
    .ride-actions {
        display: flex;
        justify-content: center;
        flex-wrap: wrap;
        margin-top: 1rem;
    }
`;
document.head.appendChild(style);
