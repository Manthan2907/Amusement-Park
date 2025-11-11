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
        const stats = await response.json();
        
        // Visitor stats
        const visitorStats = document.getElementById('visitor-stats');
        visitorStats.innerHTML = `
            <p><strong>Total Visitors:</strong> ${stats.total_visitors}</p>
            <p><strong>Premium Visitors:</strong> ${stats.premium_visitors}</p>
            <p><strong>Avg Satisfaction:</strong> ${Math.round(stats.avg_satisfaction)}%</p>
            <p><strong>Total Distance:</strong> ${stats.total_distance}m</p>
        `;
        
        // Ride stats
        const rideStats = document.getElementById('ride-stats');
        rideStats.innerHTML = `
            <p><strong>Total Rides:</strong> ${stats.total_rides}</p>
            <p><strong>Active Rides:</strong> ${stats.active_rides}</p>
            <p><strong>Avg Wait Time:</strong> ${stats.avg_wait_time} min</p>
            <p><strong>Total Capacity:</strong> ${stats.total_capacity} people</p>
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
        console.error('Error loading statistics:', error);
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

// Enjoy ride
async function enjoyRide(rideId) {
    const visitorId = prompt('Enter your Visitor ID (must be 1000 or higher):');
    if (!visitorId || parseInt(visitorId) < 1000) {
        showNotification('Invalid visitor ID. Must be 1000 or higher.', 'error');
        return;
    }
    
    try {
        const response = await fetch(`${API_BASE}/rides/${rideId}/experience`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ visitor_id: parseInt(visitorId) })
        });
        
        if (response.ok) {
            const data = await response.json();
            showNotification(`🎉 ${data.message} ${data.visitor_name}'s Satisfaction: ${Math.round(data.satisfaction)}%`, 'success');
            loadVisitors(); // Refresh visitor list to show updated satisfaction
            loadDashboard(); // Refresh stats
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
        showNotification('Error enjoying ride', 'error');
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
