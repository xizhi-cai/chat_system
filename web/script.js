let socket = null;
let username = '';
let connected = false;

// 模拟WebSocket连接 (实际项目中需要真正的WebSocket服务器)
function connect() {
    const nameInput = document.getElementById('nameInput');
    username = nameInput.value.trim();
    
    if (!username) {
        alert('请输入昵称');
        return;
    }
    
    // 模拟连接成功
    connected = true;
    updateUI();
    addMessage('系统', `欢迎 ${username} 进入聊天室！`, 'system');
    
    // 模拟其他用户
    updateUserList(['Alice', 'Bob', username]);
}

function disconnect() {
    connected = false;
    updateUI();
    addMessage('系统', '您已断开连接', 'system');
    updateUserList([]);
}

function sendMessage() {
    const messageInput = document.getElementById('messageInput');
    const message = messageInput.value.trim();
    
    if (!message) return;
    
    addMessage(username, message, 'own');
    messageInput.value = '';
    
    // 模拟收到其他用户的回复
    setTimeout(() => {
        const responses = [
            '收到！',
            '好的，明白了',
            '哈哈，有意思',
            '赞同！',
            '我也这么认为'
        ];
        const randomResponse = responses[Math.floor(Math.random() * responses.length)];
        const senders = ['Alice', 'Bob'];
        const randomSender = senders[Math.floor(Math.random() * senders.length)];
        
        addMessage(randomSender, randomResponse, 'other');
    }, 1000 + Math.random() * 2000);
}

function addMessage(sender, content, type) {
    const messagesDiv = document.getElementById('messages');
    const messageDiv = document.createElement('div');
    messageDiv.className = `message ${type}`;
    
    const now = new Date();
    const timeString = now.toLocaleTimeString('zh-CN', {
        hour12: false,
        hour: '2-digit',
        minute: '2-digit',
        second: '2-digit'
    });
    
    if (type === 'system') {
        messageDiv.innerHTML = `
            <div class="message-content">${content}</div>
        `;
    } else {
        messageDiv.innerHTML = `
            <div class="message-header">${sender} ${timeString}</div>
            <div class="message-content">${content}</div>
        `;
    }
    
    messagesDiv.appendChild(messageDiv);
    messagesDiv.scrollTop = messagesDiv.scrollHeight;
}

function updateUserList(users) {
    const userListElement = document.getElementById('userList');
    userListElement.innerHTML = '';
    
    users.forEach(user => {
        const li = document.createElement('li');
        li.textContent = user;
        if (user === username) {
            li.style.fontWeight = 'bold';
            li.style.color = '#007bff';
        }
        userListElement.appendChild(li);
    });
}

function updateUI() {
    const status = document.getElementById('status');
    const usernameElement = document.getElementById('username');
    const loginForm = document.getElementById('loginForm');
    const chatForm = document.getElementById('chatForm');
    
    if (connected) {
        status.textContent = '在线';
        status.className = 'status connected';
        usernameElement.textContent = username;
        loginForm.style.display = 'none';
        chatForm.style.display = 'flex';
    } else {
        status.textContent = '离线';
        status.className = 'status';
        usernameElement.textContent = '未登录';
        loginForm.style.display = 'flex';
        chatForm.style.display = 'none';
    }
}

// 事件监听
document.getElementById('nameInput').addEventListener('keypress', function(e) {
    if (e.key === 'Enter') {
        connect();
    }
});

document.getElementById('messageInput').addEventListener('keypress', function(e) {
    if (e.key === 'Enter') {
        sendMessage();
    }
});

// 初始化
document.addEventListener('DOMContentLoaded', function() {
    updateUI();
    addMessage('系统', '欢迎使用Socket通信软件！请输入昵称连接服务器。', 'system');
});