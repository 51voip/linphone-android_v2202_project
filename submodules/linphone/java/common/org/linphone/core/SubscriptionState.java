package org.linphone.core;

public enum SubscriptionState {
	/**
	 * Initial state, should not be used.
	 */
	None(0), 
	/**
	 * An outgoing subcription was created.
	 */
	OutoingInit(1),
	/**
	 * An incoming subcription is received.
	 */
	IncomingReceived(2), 
	/**
	 * Subscription is pending, waiting for user approval
	 */
	Pending(3),
	/**
	 * Subscription is accepted and now active.
	 */
	Active(4),
	/**
	 * Subscription is terminated normally
	 */
	Terminated(5), 
	/**
	 * Subscription encountered an error, indicated by { @link LinphoneEvent.getReason() }
	 */
	Error(6);
	
	protected final int mValue;
	private SubscriptionState(int value){
		mValue=value;
	}
	static protected SubscriptionState fromInt(int value) throws LinphoneCoreException{
		switch(value){
		case 0: return None;
		case 1: return OutoingInit;
		case 2: return IncomingReceived;
		case 3: return Pending;
		case 4: return Active;
		case 5: return Terminated;
		case 6: return Error;
		default:
			throw new LinphoneCoreException("Unhandled enum value "+value+" for SubscriptionState");
		}
	}
}
