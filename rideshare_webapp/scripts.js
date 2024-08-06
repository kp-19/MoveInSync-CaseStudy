function navigateTo(page) {
    window.location.href = page;
}

function openLoginModal() {
    document.getElementById('loginModal').style.display = 'block';
}

function closeLoginModal() {
    document.getElementById('loginModal').style.display = 'none';
}

function openTravelerCompanionLogin() {
    document.getElementById('loginModal').style.display = 'none';
    document.getElementById('travelerCompanionLogin').style.display = 'block';
}

function closeTravelerCompanionLogin() {
    document.getElementById('travelerCompanionLogin').style.display = 'none';
}

function openAdminLogin() {
    document.getElementById('loginModal').style.display = 'none';
    document.getElementById('adminLogin').style.display = 'block';
}

function closeAdminLogin() {
    document.getElementById('adminLogin').style.display = 'none';
}

// Close the modal if the user clicks outside of it
window.onclick = function(event) {
    const loginModal = document.getElementById('loginModal');
    const travelerCompanionLogin = document.getElementById('travelerCompanionLogin');
    const adminLogin = document.getElementById('adminLogin');
    
    if (event.target === loginModal) {
        loginModal.style.display = 'none';
    } else if (event.target === travelerCompanionLogin) {
        travelerCompanionLogin.style.display = 'none';
    } else if (event.target === adminLogin) {
        adminLogin.style.display = 'none';
    }
}
